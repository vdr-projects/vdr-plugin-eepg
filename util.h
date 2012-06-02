/*
 * util.h
 *
 *  Created on: 23.5.2012
 *      Author: d.petrovski
 */

#ifndef UTIL_H_
#define UTIL_H_
#include <time.h>
class cChannel;
struct tChannelID;
class cEvent;

namespace util
{
extern int AvailableSources[32];
extern int NumberOfAvailableSources;

extern int Yesterday;
extern int YesterdayEpoch;
extern int YesterdayEpochUTC;

void AddEvent(cEvent *event, tChannelID ChannelID);

cChannel *GetChannelByID(tChannelID & channelID, bool searchOtherPos);
time_t LocalTime2UTC (time_t t);
time_t UTC2LocalTime (time_t t);
void GetLocalTimeOffset (void);
void CleanString (unsigned char *String);
}
#endif /* UTIL_H_ */
