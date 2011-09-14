/***************************************************************************
 *                                                                         *
 *   These routines decompress Huffman coded Dish Network EIT data.        *
 *   The implementation is based on the algorithm presentend in            *
 *                                                                         *
 *     "A memory-efficient Huffman decoding algorithm"                     *
 *     Pi-Chung Wang, Yuan-Rung Yang, Chun-Liang Lee, Hung-Yi Chang        *
 *     Proceedings of the 19th International Conference on Advanced        *
 *     Information Networking and Applications (AINA'05)                   *
 *                                                                         *
 ***************************************************************************/

#include "dish.h"
#include <libsi/si.h>
#include <libsi/descriptor.h>
#include <string.h>
#include <stdlib.h>

namespace SI
{

  // returns the value of a sequence of bits in the byte array
  static unsigned int getBits(int bitIndex, int bitCount, const unsigned char *byteptr, int length)
  {
     union {
        unsigned char b[4];
        unsigned long val;
     } chunk;

     int offset = bitIndex >> 3;
     int bitnum = bitIndex - (offset << 3);
     int rightend = 32 - bitnum - bitCount;

     chunk.b[3] = byteptr[offset];
     chunk.b[2] = (offset+1 < length) ? byteptr[offset+1] : 0;
     chunk.b[1] = (offset+2 < length) ? byteptr[offset+2] : 0;
     chunk.b[0] = 0; // Never need to look this far ahead.

     return (unsigned int)(((chunk.val & (0xFFFFFFFF >> bitnum)) >> rightend));
    }

    DishDescriptor::DishDescriptor(UnimplementedDescriptor *unimplementedDesc)
    {
        text = NULL;
        shortText = NULL;
        decompressed = NULL;
        this->unimplementedDesc = unimplementedDesc;
    }

    DishDescriptor::~DishDescriptor()
    {
        delete [] decompressed;
        decompressed = NULL;
        delete unimplementedDesc;
    }

    const char *DishDescriptor::getTheme(int contentNibleLvl2)
    {
      const char* theme;
      using namespace DISH_THEMES;

      switch (contentNibleLvl2) {
        case Movie:
          theme = "Movie";
          break;
        case Sports:
          theme = "Sports";
            break;
        case News_Business:
          theme = "News/Business";
            break;
        case Family_Children:
          theme = "Family/Children";
            break;
        case Education:
          theme = "Education";
            break;
        case Series_Special:
          theme = "Series/Special";
            break;
        case Music_Art:
          theme = "Music/Art";
            break;
        case Religious:
          theme = "Religious";
            break;
        default:
          theme = "";
          break;
      }
      return theme;
    }

    const char *DishDescriptor::getCategory(int userNible)
    {
      using namespace DISH_CATEGORIES;

      switch (userNible) {
      case Action: return "Action";
      case ActionSports:              return "Action Sports";
      case Adults_only:               return "Adults only";
      case Adventure:                 return "Adventure";
      case Agriculture:               return "Agriculture";
      case AirRacing:                 return "Air racing";
      case Animals:                   return "Animals";
      case Animated:                  return "Animated";
      case Anime:                     return "Anime";
      case Anthology:                 return "Anthology";
      case ArmWrestling:              return "Arm wrestling";
      case Art:                       return "Art";
      case Arts_crafts:               return "Arts/crafts";
      case Auction:                   return "Auction";
      case Auto:                      return "Auto";
      case AutoRacing:                return "Auto racing";
      case Awards:                    return "Awards";
      case Badminton:                 return "Badminton";
      case Ballet:                    return "Ballet";
      case Baseball:                  return "Baseball";
      case Basketball:                return "Basketball";
      case BicycleRacing:             return "Bicycle racing";
      case Biography:                 return "Biography";
      case Boat:                      return "Boat";
      case BoatRacing:                return "Boat racing";
      case Bowling:                   return "Bowling";
      case Boxing:                    return "Boxing";
      case Bus_financial:             return "Bus./financial";
      case CardGames:                 return "Card games";
      case Children:                  return "Children";
      case ChildrenMusic:             return "Children music";
      case ChildrenNews:              return "Children news";
      case ChildrenSpecial:           return "Children special";
      case Collectibles:              return "Collectibles";
      case Comedy:                    return "Comedy";
      case ComedyDrama:               return "Comedy-drama";
      case Community:                 return "Community";
      case Computers:                 return "Computers";
      case Consumer:                  return "Consumer";
      case Cooking:                   return "Cooking";
      case Crime:                     return "Crime";
      case CrimeDrama:                return "Crime drama";
      case Dance:                     return "Dance";
      case Debate:                    return "Debate";
      case DishNetwork:               return "Dish Network";
      case Docudrama:                 return "Docudrama";
      case Documentary:               return "Documentary";
      case DogShow:                   return "DogShow";
      case DragRacing:                return "DragRacing";
      case Drama:                     return "Drama";
      case Educational:               return "Educational";
      case Entertainment:             return "Entertainment";
      case Environment:               return "Environment";
      case Equestrian:                return "Equestrian";
      case Excercise:                 return "Excercise";
      case Fantasy:                   return "Fantasy";
      case Fashion:                   return "Fashion";
      case FieldHockey:               return "Field hockey";
      case Fishing:                   return "Fishing";
      case Football:
      case Football2:                 return "Football";
      case French:                    return "French";
      case Fundraiser:                return "Fundraiser";
      case GameShow:                  return "GameShow";
      case Gay_lesbian:               return "Gay/lesbian";
      case Golf:                      return "Golf";
      case Gymnastics:                return "Gymnastics";
      case Handball:                  return "Handball";
      case Health:                    return "Health";
      case HistoricalDrama:           return "Historical drama";
      case History:                   return "History";
      case Hockey:                    return "Hockey";
      case Holiday:                   return "Holiday";
      case HolidayChildren:           return "Holiday children";
      case HolidayChildrenSpecial:    return "Holiday children special";
      case HolidaySpecial:            return "Holiday special";
      case HomeImprovement:           return "Home improvement";
      case Horror:                    return "Horror";
      case HorseRacing:               return "Horse racing";
      case House_garden:              return "House/garden";
      case HowTo:                     return "HowTo";
      case Hunting:                   return "Hunting";
      case HydroplaneRacing:          return "Hydroplane racing";
      case Interview:                 return "Interview";
      case Lacrosse:                  return "Lacrosse";
      case Law:                       return "Law";
      case MartialArts:               return "Martial arts";
      case Medical:                   return "Medical";
      case Military:                  return "Military";
      case Miniseries:                return "Miniseries";
      case MixedMartialArts:          return "Mixed martial arts";
      case Motorcycle:                return "Motorcycle";
      case MotorcycleRacing:          return "Motorcycle racing";
      case Motorsports:               return "Motorsports";
      case Music:                     return "Music";
      case MusicSpecial:              return "Music special";
      case MusicTalk:                 return "Music talk";
      case Musical:                   return "Musical";
      case MusicalComedy:             return "Musical comedy";
      case Mystery:                   return "Mystery";
      case Nature:                    return "Nature";
      case News:                      return "News";
      case Newsmagazine:              return "Newsmagazine";
      case Opera:                     return "Opera";
      case Outdoors:                  return "Outdoors";
      case Paranormal:                return "Paranormal";
      case Parenting:                 return "Parenting";
      case PerformingArts:            return "Performing arts";
      case Poker:                     return "Poker";
      case Politics:                  return "Politics";
      case ProWrestling:              return "Pro wrestling";
      case PublicAffairs:             return "Public affairs";
      case Reality:                   return "Reality";
      case Religious:                 return "Religious";
      case Rodeo:                     return "Rodeo";
      case Romance:                   return "Romance";
      case RomanceComedy:             return "Romance comedy";
      case Rugby:                     return "Rugby";
      case Running:                   return "Running";
      case Sailing:                   return "Sailing";
      case Science:                   return "Science";
      case ScienceFiction:            return "Science fiction";
      case SelfImprovement:           return "Self improvement";
      case Shooting:                  return "Shooting";
      case Shopping:                  return "Shopping";
      case Sitcom:                    return "Sitcom";
      case Skateboarding:             return "Skateboarding";
      case Skiing:                    return "Skiing";
      case Snowboarding:              return "Snowboarding";
      case Soap:                      return "Soap";
      case Soccor:                    return "Soccor";
      case Softball:                  return "Softball";
      case Spanish:                   return "Spanish";
      case Special:                   return "Special";
      case SportsNonEvent:            return "SportsNonEvent";
      case SportsTalk:                return "SportsTalk";
      case Standup:                   return "Standup";
      case Surfing:                   return "Surfing";
      case Suspense:                  return "Suspense";
      case Swimming:                  return "Swimming";
      case Talk:                      return "Talk";
      case Technology:                return "Technology";
      case Tennis:
      case Tennis2:                   return "Tennis";
      case Track_field:               return "Track/field";
      case Travel:                    return "Travel";
      case Triathlon:                 return "Triathlon";
      case Variety:                   return "Variety";
      case Volleyball:                return "Volleyball";
      case War:                       return "War";
      case Watersports:               return "Watersports";
      case Weather:                   return "Weather";
      case Western:                   return "Western";
      case Wrestling:                 return "Wrestling";
      case Yoga:                      return "Yoga";
      default: return "";
      }

    }

    void DishDescriptor::Decompress(unsigned char Tid)
    {
        const unsigned char *str = unimplementedDesc->getData().getData();
        const unsigned char *cmp = NULL; // Compressed data
        int length = 0; // Length of compressed data
        unsigned int dLength = 0; // Length of decompressed data
        if((str[3] & 0xFC) == 0x80){
            length = str[1] - 2;
            dLength = (str[2] & 0x40) ? ((str[3] << 6) & 0xFF) | (str[2] & 0x3F) : str[2] & 0x3F;
            cmp = str + 4;
        }else{
            length = str[1] - 1;
            dLength = str[2] & 0x7F;
            cmp = str + 3;
        }
        if(length <= 0 || !dLength)
            return;

        decompressed = new unsigned char[dLength + 1];
        HuffmanTable *table;
        unsigned int tableSize, numBits;
        if (Tid > 0x80) {
        table = Table255;
        tableSize = SIZE_TABLE_255;
        numBits = 13;
     }
     else {
        table = Table128;
        tableSize = SIZE_TABLE_128;
        numBits = 11;
     }
        unsigned int bLength = length << 3; // number of bits
        unsigned int currentBit = 0, count = 0;
        while(currentBit < bLength - 1 && count < dLength){
            // Find the interval containing the sequence of length numBits starting
            // at currentBit. The corresponding character will  be the one encoded
            // at the begin of the sequence.
            unsigned int code = getBits(currentBit, numBits, cmp, length);
            // We could use a binary search, but in practice this linear search is faster.
            unsigned int index = 0;
            while(table[index].startingAddress <= code && index < tableSize){
                index++;
            }
            index--;
            decompressed[count++] = table[index].character;
            currentBit += table[index].numberOfBits;
        }

        decompressed[count] = 0;
        char *split = strchr((char*)(decompressed), 0x0D); // Look for carriage return
        //LogD(2, prep("dLength:%d, length:%d, count:%d, decompressed: %s"), dLength, length, count, decompressed);
        if(split){
            *split = 0;
            shortText = (char*)(decompressed);
            text = (split[1] == 0x20) ? split + 2 : split + 1;
        }else{
            text = (char*)(decompressed);
        }
    }

    struct DishDescriptor::HuffmanTable DishDescriptor::Table128[SIZE_TABLE_128] = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}};
    struct DishDescriptor::HuffmanTable DishDescriptor::Table255[SIZE_TABLE_255] = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
  };

} /* namespace SI */
