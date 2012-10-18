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
class cEquivHandler;
class cSchedules;

#define START   '\0'
#define STOP    '\0'
#define ESCAPE  '\1'

#define Asprintf(a, b, c...) void( asprintf(a, b, c) < 0 ? esyslog("memory allocation error - %s", b) : void() )

namespace util
{

extern int AvailableSources[32];
extern int NumberOfAvailableSources;

extern int Yesterday;
extern int YesterdayEpoch;
extern int YesterdayEpochUTC;

extern enum EFormat
{
//First all batchmode, load ONCE protocols:
  MHW1 = 0,
  MHW2,
  SKY_IT,
  SKY_UK,
  NAGRA,
//Than all CONTinuous protocols, so they will be processed LAST:
  PREMIERE,
  FREEVIEW,
  DISH_BEV,
  EIT,
//the highest number of EPG-formats that is supported by this plugin
  HIGHEST_FORMAT = EIT
} Format;

extern cEquivHandler* EquivHandler;

void AddEvent(cEvent *event, tChannelID ChannelID);

cChannel *GetChannelByID(tChannelID & channelID, bool searchOtherPos);
time_t LocalTime2UTC(time_t t);
time_t UTC2LocalTime(time_t t);
void GetLocalTimeOffset(void);
void CleanString(unsigned char *String);
void decodeText2(const unsigned char *from, int len, char *buffer, int buffsize);
char *freesat_huffman_decode(const unsigned char *src, size_t size);
void sortSchedules(cSchedules * Schedules, tChannelID channelID);

//struct sNode
//{
//  char *Value;
//  struct sNode *P0;
//  struct sNode *P1;
//};
//
//typedef struct sNode sNodeH;

template<class T> T REALLOC(T Var, size_t Size)
{
  T p = (T) realloc(Var, Size);
  if (!p) free(Var);
  return p;
}

struct hufftab
{
  unsigned int value;
  short bits;
  char next;
};

extern struct hufftab *tables[2][128];
extern int table_size[2][128];
//static sNodeH* sky_tables[2];

}
#endif /* UTIL_H_ */
