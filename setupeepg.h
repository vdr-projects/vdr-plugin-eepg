/*
 * setupeepg.h
 *
 *  Created on: 08.5.2012
 *      Author: d.petrovski
 */

#ifndef SETUPEEPG_H_
#define SETUPEEPG_H_

class cSetupEEPG
{
public:
  int OptPat;
  int OrderInfo;
  int RatingInfo;
  int FixEpg;
  int DisplayMessage;
  int ProcessEIT;
#ifdef DEBUG
  int LogLevel;
#endif

public:
  static cSetupEEPG* getInstance();

private:
  cSetupEEPG (void);
  cSetupEEPG* _setupEEPG;

};

#endif /* SETUPEEPG_H_ */
