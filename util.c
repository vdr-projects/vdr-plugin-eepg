/*
 * util.c
 *
 *  Created on: 23.5.2012
 *      Author: d.petrovski
 */
#include "util.h"
#include "log.h"
#include "equivhandler.h"
#include <vdr/channels.h>
#include <vdr/thread.h>
#include <vdr/epg.h>

namespace util
{

int AvailableSources[32];
int NumberOfAvailableSources = 0;

int Yesterday;
int YesterdayEpoch;
int YesterdayEpochUTC;

struct hufftab *tables[2][128];
int table_size[2][128];

EFormat Format;
cEquivHandler* EquivHandler;

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
  if (Spaces > 0 && String && String < Dst) {
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
  LogD (0, prep("Action"));
  SetPriority(19);
  while (Running() && !LastHandleEvent.TimedOut()) {
//     LogD (0, prep("Running"));
     cAddEventListItem *e = NULL;
     cSchedulesLock SchedulesLock(true, 10);
     cSchedules *schedules = (cSchedules *)cSchedules::Schedules(SchedulesLock);
     Lock();
     while (schedules && (e = list->First()) != NULL) {
//       tChannelID chID = e->GetChannelID();
           cSchedule *schedule = (cSchedule *)schedules->GetSchedule(Channels.GetByChannelID(e->GetChannelID()), true);
//       while (schedules && (e = list->First()) != NULL) {

//         if (chID == e->GetChannelID()) {
//           LogD (0, prep("AddEvent"));
           schedule->AddEvent(e->GetEvent());
//           LogD (0, prep("Del"));
           list->Del(e);
//         }
//       }
           LogD (0, prep("Sort"));
           EpgHandlers.SortSchedule(schedule);
           EpgHandlers.DropOutdated(schedule, e->GetEvent()->StartTime(), e->GetEvent()->EndTime(), e->GetEvent()->TableID(), e->GetEvent()->Version());
//           LogD (0, prep("Sorted"));
           }
//           LogD (0, prep("Unlock"));
     Unlock();
     cCondWait::SleepMs(10);
     }
}

void cAddEventThread::AddEvent(cEvent *Event, tChannelID ChannelID)
{
  LOCK_THREAD;
//  LogD (0, prep("AddEventT %s channel: <%s>"), Event->Title(), *ChannelID.ToString());
  list->Add(new cAddEventListItem(Event, ChannelID));
  LastHandleEvent.Set(INSERT_TIMEOUT_IN_MS);
}

static cAddEventThread AddEventThread;

// ---

void AddEvent(cEvent *Event, tChannelID ChannelID)
{
  LogD (0, prep("AddEvent %s channel: <%s>"), Event->Title(), *ChannelID.ToString());
  AddEventThread.AddEvent(Event, ChannelID);
  if (!AddEventThread.Active())
     AddEventThread.Start();
}

/** \brief Decode an EPG string as necessary
 *
 *  \param src - Possibly encoded string
 *  \param size - Size of the buffer
 *
 *  \retval NULL - Can't decode
 *  \return A decoded string
 */
char *freesat_huffman_decode (const unsigned char *src, size_t size)
{
  int tableid;
//  freesat_decode_error = 0;

  if (src[0] == 0x1f && (src[1] == 1 || src[1] == 2)) {
    int uncompressed_len = 30;
    char *uncompressed = (char *) calloc (1, uncompressed_len + 1);
    unsigned value = 0, byte = 2, bit = 0;
    int p = 0;
    unsigned char lastch = START;

    tableid = src[1] - 1;
    while (byte < 6 && byte < size) {
      value |= src[byte] << ((5 - byte) * 8);
      byte++;
    }
    //freesat_table_load ();    /**< Load the tables as necessary */

    do {
      bool found = false;
      unsigned bitShift = 0;
      if (lastch == ESCAPE) {
        char nextCh = (value >> 24) & 0xff;
        found = true;
        // Encoded in the next 8 bits.
        // Terminated by the first ASCII character.
        bitShift = 8;
        if ((nextCh & 0x80) == 0)
          lastch = nextCh;
        if (p >= uncompressed_len) {
          uncompressed_len += 10;
          uncompressed = (char *) REALLOC (uncompressed, uncompressed_len + 1);
        }
        uncompressed[p++] = nextCh;
        uncompressed[p] = 0;
      } else {
        int j;
        for (j = 0; j < table_size[tableid][lastch]; j++) {
          unsigned mask = 0, maskbit = 0x80000000;
          short kk;
          for (kk = 0; kk < tables[tableid][lastch][j].bits; kk++) {
            mask |= maskbit;
            maskbit >>= 1;
          }
          if ((value & mask) == tables[tableid][lastch][j].value) {
            char nextCh = tables[tableid][lastch][j].next;
            bitShift = tables[tableid][lastch][j].bits;
            if (nextCh != STOP && nextCh != ESCAPE) {
              if (p >= uncompressed_len) {
                uncompressed_len += 10;
                uncompressed = (char *) REALLOC (uncompressed, uncompressed_len + 1);
              }
              uncompressed[p++] = nextCh;
              uncompressed[p] = 0;
            }
            found = true;
            lastch = nextCh;
            break;
          }
        }
      }
      if (found) {
        // Shift up by the number of bits.
        unsigned b;
        for (b = 0; b < bitShift; b++) {
          value = (value << 1) & 0xfffffffe;
          if (byte < size)
            value |= (src[byte] >> (7 - bit)) & 1;
          if (bit == 7) {
            bit = 0;
            byte++;
          } else
            bit++;
        }
      } else {
        LogE (0, prep("Missing table %d entry: <%s>"), tableid + 1, uncompressed);
        // Entry missing in table.
        return uncompressed;
      }
    } while (lastch != STOP && value != 0);

    return uncompressed;
  }
  return NULL;
}

void decodeText2 (const unsigned char *from, int len, char *buffer, int buffsize)
{
  if (from[0] == 0x1f) {
    char *temp = freesat_huffman_decode (from, len);
    if (temp) {
      len = strlen (temp);
      len = len < buffsize - 1 ? len : buffsize - 1;
      strncpy (buffer, temp, len);
      buffer[len] = 0;
      free (temp);
      return;
    }
  }

  SI::String convStr;
  SI::CharArray charArray;
  charArray.assign(from, len);
  convStr.setData(charArray, len);
  //LogE(5, prep("decodeText2 from %s - length %d"), from, len);
  convStr.getText(buffer,  buffsize);
  //LogE(5, prep("decodeText2 buffer %s - buffsize %d"), buffer, buffsize);
}

void sortSchedules(cSchedules * Schedules, tChannelID channelID){

  LogD(3, prep("Start sortEquivalent %s"), *channelID.ToString());

  cChannel *pChannel = GetChannelByID (channelID, false);
  cSchedule *pSchedule;
  if (pChannel) {
    pSchedule = (cSchedule *) (Schedules->GetSchedule(pChannel, true));
      pSchedule->Sort();
      Schedules->SetModified(pSchedule);
    }
  if (EquivHandler->getEquiChanMap().count(*channelID.ToString()) > 0)
    EquivHandler->sortEquivalents(channelID, Schedules);
}

}

