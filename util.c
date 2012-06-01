/*
 * util.c
 *
 *  Created on: 23.5.2012
 *      Author: d.petrovski
 */
#include "util.h"
#include <vdr/channels.h>
#include <vdr/thread.h>

namespace util
{

cChannel *GetChannelByID(tChannelID & channelID, bool searchOtherPos)
{
  cChannel *VC = Channels.GetByChannelID(channelID, true);
  if(!VC && searchOtherPos){
    //look on other satpositions
    for(int i = 0;i < NumberOfAvailableSources;i++){
      channelID = tChannelID(AvailableSources[i], channelID.Nid(), channelID.Tid(), channelID.Sid());
      VC = Channels.GetByChannelID(channelID, true);
      if(VC){
        //found this actually on satellite nextdoor...
        break;
      }
    }
  }

  return VC;
}

/*
 * Convert local time to UTC
 */
time_t LocalTime2UTC (time_t t)
{
  struct tm *temp;

  temp = gmtime (&t);
  temp->tm_isdst = -1;
  return mktime (temp);
}

/*
 * Convert UTC to local time
 */
time_t UTC2LocalTime (time_t t)
{
  return 2 * t - LocalTime2UTC (t);
}

void GetLocalTimeOffset (void)
{
  time_t timeLocal;
  struct tm *tmCurrent;

  timeLocal = time (NULL);
  timeLocal -= 86400;
  tmCurrent = gmtime (&timeLocal);
  Yesterday = tmCurrent->tm_wday;
  tmCurrent->tm_hour = 0;
  tmCurrent->tm_min = 0;
  tmCurrent->tm_sec = 0;
  tmCurrent->tm_isdst = -1;
  YesterdayEpoch = mktime (tmCurrent);
  YesterdayEpochUTC = UTC2LocalTime (mktime (tmCurrent));
}

void CleanString (unsigned char *String)
{

//  LogD (1, prep("Unclean: %s"), String);
  unsigned char *Src;
  unsigned char *Dst;
  int Spaces;
  int pC;
  Src = String;
  Dst = String;
  Spaces = 0;
  pC = 0;
  while (*Src) {
    // corrections
    if (*Src == 0x8c) { // iso-8859-2 LATIN CAPITAL LETTER S WITH ACUTE
      *Src = 0xa6;
    }
    if (*Src == 0x8f) { // iso-8859-2 LATIN CAPITAL LETTER Z WITH ACUTE
      *Src = 0xac;
    }

    if (*Src!=0x0A &&  *Src < 0x20) { //don't remove newline
      *Src = 0x20;
    }
    if (*Src == 0x20) {
      Spaces++;
      if (pC == 0) {
        Spaces++;
      }
    } else {
      Spaces = 0;
    }
    if (Spaces < 2) {
      *Dst = *Src;
      Dst++;
      pC++;
    }
    Src++;
  }
  if (Spaces > 0) {
    Dst--;
    *Dst = 0;
  } else {
    *Dst = 0;
  }
//  LogD (1, prep("Clean: %s"), String);
}

// --- cAddEventThread ----------------------------------------
// Taken from VDR EPGFixer Plug-in
// http://projects.vdr-developer.org/projects/plg-epgfixer
// by  Matti Lehtimaki

class cAddEventListItem : public cListObject
{
protected:
  cEvent *event;
  tChannelID channelID;
public:
  cAddEventListItem(cEvent *Event, tChannelID ChannelID) { event = Event; channelID = ChannelID; }
  tChannelID GetChannelID() { return channelID; }
  cEvent *GetEvent() { return event; }
  ~cAddEventListItem() { }
};

class cAddEventThread : public cThread
{
private:
  cTimeMs LastHandleEvent;
  cList<cAddEventListItem> *list;
  enum { INSERT_TIMEOUT_IN_MS = 10000 };
protected:
  virtual void Action(void);
public:
  cAddEventThread(void);
  ~cAddEventThread(void);
  void AddEvent(cEvent *Event, tChannelID ChannelID);
};

cAddEventThread::cAddEventThread(void)
:cThread("cAddEventThread"), LastHandleEvent()
{
  list = new cList<cAddEventListItem>;
}

cAddEventThread::~cAddEventThread(void)
{
  LOCK_THREAD;
  list->cList::Clear();
  Cancel(3);
}

void cAddEventThread::Action(void)
{
  SetPriority(19);
  while (Running() && !LastHandleEvent.TimedOut()) {
     cAddEventListItem *e = NULL;
     cSchedulesLock SchedulesLock(true, 10);
     cSchedules *schedules = (cSchedules *)cSchedules::Schedules(SchedulesLock);
     Lock();
     while (schedules && (e = list->First()) != NULL) {
           cSchedule *schedule = (cSchedule *)schedules->GetSchedule(Channels.GetByChannelID(e->GetChannelID()), true);
           schedule->AddEvent(e->GetEvent());
           EpgHandlers.SortSchedule(schedule);
           EpgHandlers.DropOutdated(schedule, e->GetEvent()->StartTime(), e->GetEvent()->EndTime(), e->GetEvent()->TableID(), e->GetEvent()->Version());
           list->Del(e);
           }
     Unlock();
     cCondWait::SleepMs(10);
     }
}

void cAddEventThread::AddEvent(cEvent *Event, tChannelID ChannelID)
{
  LOCK_THREAD;
  list->Add(new cAddEventListItem(Event, ChannelID));
  LastHandleEvent.Set(INSERT_TIMEOUT_IN_MS);
}

static cAddEventThread AddEventThread;

// ---

void AddEvent(cEvent *Event, tChannelID ChannelID)
{
  AddEventThread.AddEvent(Event, ChannelID);
  if (!AddEventThread.Active())
     AddEventThread.Start();
}


}

