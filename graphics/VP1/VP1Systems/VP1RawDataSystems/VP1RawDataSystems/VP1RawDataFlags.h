/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Common flags used in the raw data system.                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAFLAGS_H
#define VP1RAWDATAFLAGS_H

#include <QFlags>

class VP1RawDataFlags {
public:

  enum InDetPartsFlag {
    None           = 0x000000,
    BarrelPositive = 0x000001,
    BarrelNegative = 0x000002,
    EndCapPositive = 0x000004,
    EndCapNegative = 0x000008,
    AllBarrel      = 0x000003,
    AllEndCap      = 0x00000C,
    All            = 0x00000F
  };
  Q_DECLARE_FLAGS(InDetPartsFlags, InDetPartsFlag)

private:
  VP1RawDataFlags();
  ~VP1RawDataFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(VP1RawDataFlags::InDetPartsFlags)

#endif
