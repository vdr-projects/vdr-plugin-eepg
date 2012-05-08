/*
 * cEEpgHandler.c
 *
 *  Created on: 11.3.2012
 *      Author: d.petrovski
 */

#include "epghandler.h"

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
}

bool cEEpgHandler::SetTitle(cEvent* Event, const char* Title) {
}

bool cEEpgHandler::SetShortText(cEvent* Event, const char* ShortText) {
}

bool cEEpgHandler::SetDescription(cEvent* Event, const char* Description) {
}

bool cEEpgHandler::SetContents(cEvent* Event, uchar* Contents) {
}

bool cEEpgHandler::SetParentalRating(cEvent* Event, int ParentalRating) {
}

bool cEEpgHandler::SetStartTime(cEvent* Event, time_t StartTime) {
}

bool cEEpgHandler::SetDuration(cEvent* Event, int Duration) {
}

bool cEEpgHandler::SetVps(cEvent* Event, time_t Vps) {
}

bool cEEpgHandler::HandleEvent(cEvent* Event) {
}

bool cEEpgHandler::SortSchedule(cSchedule* Schedule) {
}

bool cEEpgHandler::DropOutdated(cSchedule* Schedule, time_t SegmentStart,
		time_t SegmentEnd, uchar TableID, uchar Version) {
}


