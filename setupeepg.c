/*
 * setupeepg.c
 *
 *  Created on: 08.5.2012
 *      Author: d.petrovski
 */

#include "setupeepg.h"

// --- cSetupEEPG -------------------------------------------------------

cSetupEEPG::cSetupEEPG (void)
{
  OptPat = 1;
  OrderInfo = 1;
  RatingInfo = 1;
  FixEpg = 0;
  DisplayMessage = 1;
  ProcessEIT = 0;
#ifdef DEBUG
  LogLevel = 0;
#endif

}

static cSetupEEPG* cSetupEEPG::getInstance()
{
	if (!_setupEEPG)
		_setupEEPG = new cSetupEEPG();

	return _setupEEPG;
}

