/*
 * equivhandler.cpp
 *
 *  Created on: 19.5.2012
 *      Author: d.petrovski
 */

#include "equivhandler.h"
#include "setupeepg.h"
#include "log.h"

#include <string>


static multimap<string, string> cEquivHandler::equiChanMap;
static long cEquivHandler::equiChanFileTime = 0;

cEquivHandler::cEquivHandler()
{
  loadEquivalentChannelMap();
}

cEquivHandler::~cEquivHandler()
{
  // TODO Auto-generated destructor stub
}

void cEquivHandler::loadEquivalentChannelMap (void)
{
  char Buffer[1024];
  char *Line;
  FILE *File;
  string FileName = string(cSetupEEPG::getInstance()->getConfDir()) + "/" + EEPG_FILE_EQUIV;
  multimap<string,string>::iterator it,it2;
  pair<multimap<string,string>::iterator,multimap<string,string>::iterator> ret;

  //Test if file is changed and reload
  struct stat st;
  if (stat(FileName.c_str(), &st)) {
      LogE(0, prep("Error obtaining stats for '%s' "), FileName.c_str());
      return;
  }

  if (equiChanMap.size() > 0 &&  equiChanFileTime == st.st_mtim.tv_nsec)
    return;
  else
    equiChanMap.clear();


  File = fopen (FileName.c_str(), "r");
  if (File) {
    memset (Buffer, 0, sizeof (Buffer));
    char origChanID[256];
    char equiChanID[256];
    char source[256];
    int nid = 0;
    int tid = 0;
    int sid = 0;
    int rid = 0;
    while ((Line = fgets (Buffer, sizeof (Buffer), File)) != NULL) {
      Line = compactspace (skipspace (stripspace (Line)));
      if (!isempty (Line)) {
        if (sscanf (Line, "%[^ ] %[^ ] %[^\n]\n", origChanID, equiChanID, source) == 3) {
          if (origChanID[0] != '#' && origChanID[0] != ';') {
            nid = 0;
            tid = 0;
            sid = 0;
            rid = 0;
            if (sscanf (origChanID, "%[^-]-%i -%i -%i ", source, &nid, &tid, &sid) == 4)
              if (sscanf (equiChanID, "%[^-]-%i -%i -%i ", source, &nid, &tid, &sid) == 4) {
                if (sscanf (origChanID, "%[^-]-%i -%i -%i -%i ", source, &nid, &tid, &sid, &rid) != 5) {
                  rid = 0;
                }
                tChannelID OriginalChID = tChannelID (cSource::FromString (source), nid, tid, sid, rid);
                bool found = false;
                //int i = 0;
                cChannel *OriginalChannel = Channels.GetByChannelID (OriginalChID, false);
                if (!OriginalChannel) {
                  LogI(2, prep("Warning, not found epg channel \'%s\' in channels.conf. Equivalency is assumed to be valid, but perhaps you should check the entry in the equivalents file"), origChanID); //TODO: skip this ing?
                  continue;
                }
                if (sscanf (equiChanID, "%[^-]-%i -%i -%i ", source, &nid, &tid, &sid) == 4) {
                  if (sscanf (equiChanID, "%[^-]-%i -%i -%i -%i ", source, &nid, &tid, &sid, &rid)
                    != 5) {
                    rid = 0;
                  }
                  tChannelID EquivChID = tChannelID (cSource::FromString (source), nid, tid, sid, rid);
                  cChannel *EquivChannel = Channels.GetByChannelID (EquivChID, false); //TODO use valid function?
                  if (EquivChannel) {
                    ret = equiChanMap.equal_range(*OriginalChID.ToString());
                    for (it=ret.first; it!=ret.second; ++it)
                      if ((*it).second ==  *OriginalChID.ToString()) {
                        found = true;
                        break;
                      }

                    if (!found) {
                      string origCh(*OriginalChID.ToString());
                      string equiCh(*EquivChID.ToString());
                      equiChanMap.insert(pair<string,string>(origCh.c_str(),equiCh.c_str()));
                      LogD(4, prep("Found %s equivalent to %s. origCh %s"), *EquivChID.ToString(), *OriginalChID.ToString(), origCh.c_str());
                      for ( it2=equiChanMap.begin() ; it2 != equiChanMap.end(); it2++ )
                        LogD(3, prep("Original ID %s <-> Equivalent ID %s"), (*it2).first.c_str(), it2->second.c_str());
                    }
                  } else
                    LogI(0, prep("Warning, not found equivalent channel \'%s\' in channels.conf"), equiChanID);
                }
              }   //if scanf string1
          }   //if string1
        }     //if scanf
      }    //if isempty
    }      //while
    fclose (File);
    equiChanFileTime = st.st_mtim.tv_nsec;
    LogD(3, prep("Loaded %i equivalents."), equiChanMap.size());
    for ( it2=equiChanMap.begin() ; it2 != equiChanMap.end(); it2++ )
      LogD(3, prep("Original ID %s <-> Equivalent ID %s"), (*it2).first.c_str(), it2->second.c_str());
  }  //if file
}

void cEquivHandler::updateEquivalent(cSchedules * Schedules, tChannelID channelID, cEvent *pEvent){
  multimap<string,string>::iterator it;
  pair<multimap<string,string>::iterator,multimap<string,string>::iterator> ret;

  LogD(3, prep("Start updateEquivalent %s"), *channelID.ToString());

  ret = equiChanMap.equal_range(*channelID.ToString());
  for (it=ret.first; it!=ret.second; ++it) {
    LogD(1, prep("equivalent channel exists"));
    tChannelID equChannelID (tChannelID::FromString((*it).second.c_str()));
    cChannel *equChannel = GetChannelByID (equChannelID, false);
    if (equChannel) {
      LogD(3, prep("found Equivalent channel %s"), *equChannelID.ToString());
      cSchedule *pSchedule = (cSchedule *) Schedules->GetSchedule (equChannel, true);
      cEvent *pEqvEvent = (cEvent *) pSchedule->GetEvent (pEvent->EventID(), pEvent->StartTime());
      if (pEqvEvent) {
        LogD(1, prep("equivalent event exists"));
        if (pEqvEvent == pEvent) {
          LogD(1, prep("equal event exists"));

        } else {
          LogD(1, prep("remove equivalent"));
          pSchedule->DelEvent(pEqvEvent);
          cEvent* newEvent = new cEvent (pEvent->EventID());
          newEvent->SetTableID (pEvent->TableID());
          newEvent->SetStartTime (pEvent->StartTime());
          newEvent->SetDuration (pEvent->Duration());
          newEvent->SetVersion (pEvent->Version());
//        newEvent->SetContents(pEvent->Contents());
          newEvent->SetParentalRating(pEvent->ParentalRating());
          newEvent->SetVps (pEvent->Vps());
          newEvent->SetTitle (pEvent->Title ());
          newEvent->SetShortText (pEvent->ShortText ());
          newEvent->SetDescription (pEvent->Description ());
//        newEvent->SetComponents (pEvent->Components());
          newEvent->FixEpgBugs ();

          pSchedule->AddEvent(newEvent);

        }

      } else {
        LogD(1, prep("equivalent event does not exist"));
        cEvent* newEvent = new cEvent (pEvent->EventID());
        newEvent->SetTableID (pEvent->TableID());
        newEvent->SetStartTime (pEvent->StartTime());
        newEvent->SetDuration (pEvent->Duration());
        newEvent->SetVersion (pEvent->Version());
//      newEvent->SetContents(pEvent->Contents());
        newEvent->SetParentalRating(pEvent->ParentalRating());
        newEvent->SetVps (pEvent->Vps());
        newEvent->SetTitle (pEvent->Title ());
        newEvent->SetShortText (pEvent->ShortText ());
        newEvent->SetDescription (pEvent->Description ());
//      newEvent->SetComponents (pEvent->Components());
        newEvent->FixEpgBugs ();

        pSchedule->AddEvent(newEvent);

      }
    }
  }
}

void cEquivHandler::sortEquivalents(tChannelID channelID, cSchedules* Schedules)
{
  multimap<string, string>::iterator it;
  pair < multimap < string, string > ::iterator, multimap < string, string
      > ::iterator > ret;
  ret = cEquivHandler::getEquiChanMap().equal_range(*channelID.ToString());
  for (it = ret.first; it != ret.second; ++it)
    {
      LogD(3, prep("equivalent channel exists"));
      tChannelID equChannelID(tChannelID::FromString((*it).second.c_str()));
      cChannel* pChannel = GetChannelByID(equChannelID, false);
      if (pChannel)
        {
          LogD(3, prep("found Equivalent channel %s"),
              *equChannelID.ToString());
          cSchedule* pSchedule = (cSchedule *) Schedules->GetSchedule(pChannel,
              true);

          pSchedule->Sort();
          Schedules->SetModified(pSchedule);
        }
    }
}
