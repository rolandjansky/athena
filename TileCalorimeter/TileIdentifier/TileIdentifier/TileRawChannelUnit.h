/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILERAWCHANNELUNIT_H
#define TILECONDITIONS_TILERAWCHANNELUNIT_H


#include <string>


class TileRawChannelUnit
{
 public:
  enum UNIT
    {
      ADCcounts                = 0, 
      PicoCoulombs             = 1, 
      CesiumPicoCoulombs       = 2,
      MegaElectronVolts        = 3,
      OnlineADCcounts          = 4,
      OnlinePicoCoulombs       = 5,
      OnlineCesiumPicoCoulombs = 6,
      OnlineMegaElectronVolts  = 7,
      OnlineOffset             = 4,
      Invalid                  = 99
    };


  /** Printable representation of a unit. */
  static std::string unitToString (UNIT type);
};

#endif
