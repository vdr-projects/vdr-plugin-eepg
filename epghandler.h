/*
 * cEEpgHandler.h
 *
 *  Created on: 11.3.2012
 *      Author: d.petrovski
 */

#ifndef CEEPGHANDLER_H_
#define CEEPGHANDLER_H_
#include <vdr/config.h>
#if APIVERSNUM > 10725
#include <vdr/epg.h>
#include <string>

class cEquivHandler;

class cEEpgHandler : public cEpgHandler {
public:
    cEEpgHandler();
    virtual ~cEEpgHandler();
    virtual bool IgnoreChannel(const cChannel *Channel) { return false; }
    virtual bool HandleEitEvent(cSchedule *Schedule, const SI::EIT::Event *EitEvent, uchar TableID, uchar Version);
    virtual bool SetEventID(cEvent *Event, tEventID EventID);
    virtual bool SetTitle(cEvent *Event, const char *Title);
    virtual bool SetShortText(cEvent *Event, const char *ShortText);
    virtual bool SetDescription(cEvent *Event, const char *Description);
    virtual bool SetContents(cEvent *Event, uchar *Contents);
    virtual bool SetParentalRating(cEvent *Event, int ParentalRating);
    virtual bool SetStartTime(cEvent *Event, time_t StartTime);
    virtual bool SetDuration(cEvent *Event, int Duration);
    virtual bool SetVps(cEvent *Event, time_t Vps);
    virtual bool FixEpgBugs(cEvent *Event) { return false; }
    virtual bool HandleEvent(cEvent *Event);
    virtual bool SortSchedule(cSchedule *Schedule);
    virtual bool DropOutdated(cSchedule *Schedule, time_t SegmentStart, time_t SegmentEnd, uchar TableID, uchar Version);

private:
    std::string origShortText;
    std::string origDescription;
    cEquivHandler* equivHandler;
    static const int _LONG_EVENT_HOURS = 10;
};

#endif /*APIVERSNUM > 10725*/
#endif /* CEEPGHANDLER_H_ */
