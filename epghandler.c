/*
 * cEEpgHandler.c
 *
 *  Created on: 11.3.2012
 *      Author: d.petrovski
 */

#include "epghandler.h"
#include "log.h"

cEEpgHandler::cEEpgHandler() {
	// TODO Auto-generated constructor stub

}

cEEpgHandler::~cEEpgHandler() {
	// TODO Auto-generated destructor stub
}

bool cEEpgHandler::HandleEitEvent(cSchedule* Schedule,
		const SI::EIT::Event* EitEvent, uchar TableID, uchar Version) {
	return true;
}

bool cEEpgHandler::SetEventID(cEvent* Event, tEventID EventID) {
	Event->SetEventID(EventID);
	return true;
}

bool cEEpgHandler::SetTitle(cEvent* Event, const char* Title) {
    LogD(1, prep("Event id:%d title:%s new title:%s"), Event->EventID(), Event->Title(), Title);

	if (!strcmp(Event->Title(),"") || (strcmp(Title,"") && strcmp(Event->Title(),Title)))
		Event->SetTitle(Title);
	return true;
}

bool cEEpgHandler::SetShortText(cEvent* Event, const char* ShortText) {
    LogD(1, prep("Event id:%d ShortText:%s new ShortText:%s"), Event->EventID(), Event->ShortText(), ShortText);

    if (!strcmp(Event->ShortText(),"") || (strcmp(ShortText,"") && strcmp(Event->ShortText(),ShortText)))
		Event->SetShortText(ShortText);
	return true;
}

bool cEEpgHandler::SetDescription(cEvent* Event, const char* Description) {
	if (!strcmp(Event->Description(),"") || (strcmp(Description,"") && strcmp(Event->Description(),Description)))
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
	return true;
}

bool cEEpgHandler::SortSchedule(cSchedule* Schedule) {
	Schedule->Sort();
	return true;
}

bool cEEpgHandler::DropOutdated(cSchedule* Schedule, time_t SegmentStart,
		time_t SegmentEnd, uchar TableID, uchar Version) {
	return false;
}


