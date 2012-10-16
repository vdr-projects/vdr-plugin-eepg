#ifndef CEIT2_H_
#define CEIT2_H_
#include <libsi/section.h>
#include <libsi/descriptor.h>
#include <libsi/si.h>
#include <vdr/epg.h>

namespace SI
{
  enum DescriptorTagExt {
    DishRatingDescriptorTag = 0x89,
    DishShortEventDescriptorTag = 0x91,
    DishExtendedEventDescriptorTag = 0x92,
    DishSeriesDescriptorTag = 0x96,
    };

// typedef InheritEnum< DescriptorTagExt, SI::DescriptorTag > ExtendedDescriptorTag;

/*extern const char *getCharacterTable(const unsigned char *&buffer, int &length, bool *isSingleByte = NULL);
extern bool convertCharacterTable(const char *from, size_t fromLength, char *to, size_t toLength, const char *fromCode);
extern bool SystemCharacterTableIsSingleByte;*/
class cEIT2:public SI::EIT
{
public:
  cEIT2 (cSchedules * Schedules, int Source, u_char Tid, const u_char * Data, bool isEITPid = false, bool OnlyRunningStatus = false);
  cEIT2 (cSchedule * Schedule);
//protected:
//  void updateEquivalent(cSchedules * Schedules, tChannelID channelID, cEvent *pEvent);
  cEvent* ProcessEitEvent(cSchedule *Schedule, const SI::EIT::Event *EitEvent, uchar TableID, uchar Version);

private:
  void ProcessEventDescriptors(bool ExternalData,
    int Source, u_char Tid, const SI::EIT::Event* SiEitEvent, cEvent* pEvent,
    cSchedules* Schedules, cChannel* channel);

private:
    bool Empty;
    bool Modified;
//    bool HasExternalData = false;
    time_t SegmentStart;
    time_t SegmentEnd;
    cSchedules* Schedules;
    cChannel* channel;

    bool OnlyRunningStatus;
  };
} //end namespace SI

#endif /* CEIT2_H_ */
