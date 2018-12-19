/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */


#include "TileIdentifier/TileRawChannelUnit.h"


/**
 * @brief Printable representation of a unit.
 */
std::string TileRawChannelUnit::unitToString (UNIT unit)
{
  switch (unit) {
#define CASE(X) case TileRawChannelUnit::X: return #X
    CASE(ADCcounts);
    CASE(PicoCoulombs);
    CASE(CesiumPicoCoulombs);
    CASE(MegaElectronVolts);
    CASE(OnlineADCcounts);
    CASE(OnlinePicoCoulombs);
    CASE(OnlineCesiumPicoCoulombs);
    CASE(OnlineMegaElectronVolts);
#undef CASE
  default: break;
  };
  return "UNKNOWN";
}
