/*
 * cEEpgHandler.c
 *
 *  Created on: 11.3.2012
 *      Author: d.petrovski
 */

#include "epghandler.h"
#if APIVERSNUM > 10725
#include "log.h"
#include "equivhandler.h"
#include <vdr/sources.h>

cEEpgHandler::cEEpgHandler() {
  LogD(4, prep("cEEpgHandler()"));
  equivHandler = new cEquivHandler();
}

cEEpgHandler::~cEEpgHandler() {
  delete equivHandler;
  equivHandler = NULL;
}

bool cEEpgHandler::HandleEitEvent(cSchedule* Schedule,
                                  const SI::EIT::Event* EitEvent, uchar TableID, uchar Version) {
  //LogD(1, prep("HandleEitEvent"));
  //DISH NID 0x1001 to 0x100B BEV 0x100 and 0x101
  //TODO move the eit handling code at least for NA providers here instead of discarding.
  int nid = Schedule->ChannelID().Nid();
  if ((nid >= 0x1001 && nid <= 0x100B) || nid == 0x101 || nid == 0x100)
    return true;

  //TODO Should it be added in setup?
  if (EitEvent->getDurationHour() > _LONG_EVENT_HOURS) {
    LogD(4, prep("Event longer than 10h Duration:%d DurationHour:%d StartTimeHour:%d"), EitEvent->getDuration(), EitEvent->getDurationHour(), EitEvent->getStartTimeHour());
    const cEvent* exEvent = Schedule->GetEventAround(EitEvent->getStartTime()+EitEvent->getDuration()/3);
    const cEvent* exEvent2 = (const cEvent*)exEvent->Next();
    if (exEvent && exEvent2 && difftime (exEvent2->EndTime(),EitEvent->getStartTime()+EitEvent->getDuration()) <= 0 ) {
      LogD(3, prep("EitEvent overrides existing events '%s', '%s' ... Skipping"), exEvent->Title(), exEvent2->Title());
      return true;
    }
  }

  return false;
  //	return true;
}

bool cEEpgHandler::SetEventID(cEvent* Event, tEventID EventID) {
  Event->SetEventID(EventID);
  return true;
}

bool cEEpgHandler::SetTitle(cEvent* Event, const char* Title) {
  LogD(3, prep("Event id:%d title:%s new title:%s"), Event->EventID(), Event->Title(), Title);

  if (!Event->Title() || (Title && (!strcmp(Event->Title(),"") || (strcmp(Title,"") && strcmp(Event->Title(),Title)))))
    Event->SetTitle(Title);
  return true;
}

bool cEEpgHandler::SetShortText(cEvent* Event, const char* ShortText) {
  LogD(3, prep("Event id:%d ShortText:%s new ShortText:%s"), Event->EventID(), Event->ShortText(), ShortText);

  if (Event->ShortText() && strcmp(Event->ShortText(),"") != 0) {
    origShortText = std::string(Event->ShortText());
  }
  else {
    origShortText.clear();
  }

  //if (!Event->ShortText() || ShortText && (!strcmp(Event->ShortText(),"") || (strcmp(ShortText,"") && strcmp(Event->ShortText(),ShortText))))
  Event->SetShortText(ShortText);
  return true;
}

bool cEEpgHandler::SetDescription(cEvent* Event, const char* Description) {
  LogD(3, prep("Event id:%d Description:%s new Description:%s"), Event->EventID(), Event->Description(), Description);

  if (Event->Description() && strcmp(Event->Description(),"") != 0)
    origDescription = Event->Description();
  else
    origDescription.clear();

  //if (!Event->Description() || Description && (!strcmp(Event->Description(),"") || (strcmp(Description,"") && strcmp(Event->Description(),Description))))
  Event->SetDescription(Description);
  return true;
}

bool cEEpgHandler::SetContents(cEvent* Event, uchar* Contents) {
  Event->SetContents(Contents);
  return true;
}

bool cEEpgHandler::SetParentalRating(cEvent* Event, int ParentalRating) {
  Event->SetParentalRating(ParentalRating);
  return true;
}

bool cEEpgHandler::SetStartTime(cEvent* Event, time_t StartTime) {
  Event->SetStartTime(StartTime);
  return true;
}

bool cEEpgHandler::SetDuration(cEvent* Event, int Duration) {
  Event->SetDuration(Duration);
  return true;
}

bool cEEpgHandler::SetVps(cEvent* Event, time_t Vps) {
  Event->SetVps(Vps);
  return true;
}

bool cEEpgHandler::HandleEvent(cEvent* Event) {

  LogD(3, prep("HandleEvent st:%s ost:%s desc:%s odesc:%s"),Event->ShortText(),origShortText.c_str(),Event->Description(),origDescription.c_str());

  //After FixEpgBugs of cEvent set the original Short Text if empty
  if (!Event->ShortText() || !strcmp(Event->ShortText(),""))
    Event->SetShortText(origShortText.c_str());

  if (!Event->Description() && !origDescription.empty()) {
    Event->SetDescription(origDescription.c_str());
  }

  if (equivHandler->getEquiChanMap().count(*Event->ChannelID().ToString()) <= 0)
    return true;

  equivHandler->updateEquivalent(Event->ChannelID(), Event);

//  cSchedulesLock SchedulesLock (true);
//  cSchedules *s = (cSchedules *) cSchedules::Schedules (SchedulesLock);
//  if (s) {
//    equivHandler->updateEquivalent(s, Event->ChannelID(), Event);
//  } else
//    LogE (0, prep("Error: could not lock schedules."));

  //TODO just to see the difference
  //else if (!origDescription.empty() && !origDescription.compare(Event->Description())) {
  //	origDescription.append(" | EIT: ");
  //	origDescription.append(Event->Description());
  //	Event->SetDescription(origDescription.c_str());
  //  }

  return true;
}

bool cEEpgHandler::SortSchedule(cSchedule* Schedule) {

  Schedule->Sort();

  //NOK
//  cSchedulesLock SchedulesLock (true);
//  cSchedules *s = (cSchedules *) cSchedules::Schedules (SchedulesLock);
//  if (s) {
//    equivHandler->sortEquivalents(Schedule->ChannelID(), s);
//  } else
//    LogE (0, prep("Error: could not lock schedules."));

  return true;
}

bool cEEpgHandler::DropOutdated(cSchedule* Schedule, time_t SegmentStart,
                                time_t SegmentEnd, uchar TableID, uchar Version) {
  return false;
}

#endif
