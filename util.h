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
void decodeText2 (const unsigned char *from, int len, char *buffer, int buffsize);
char *freesat_huffman_decode (const unsigned char *src, size_t size);

struct sNode
{
  char *Value;
  struct sNode *P0;
  struct sNode *P1;
};

typedef struct sNode sNodeH;

struct hufftab {
  unsigned int value;
  short bits;
  char next;
};

#define START   '\0'
#define STOP    '\0'
#define ESCAPE  '\1'

static struct hufftab *tables[2][128];
static int table_size[2][128];
static sNodeH* sky_tables[2];


#define Asprintf(a, b, c...) void( asprintf(a, b, c) < 0 ? esyslog("memory allocation error - %s", b) : void() )

}
#endif /* UTIL_H_ */
