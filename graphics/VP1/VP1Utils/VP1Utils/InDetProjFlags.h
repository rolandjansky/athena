/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Common flags for Inner Detector projections               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

//NB: This header file is intended to be seen by other systems (so
//    they can use the flag types as signal parameters) - thus don't add
//    extra includes/functionality here!

#ifndef INDETPROJECTIONFLAGS_H
#define INDETPROJECTIONFLAGS_H

#include <QFlags>

class InDetProjFlags {
public:

  //Projected data in which parts of the detector:
   enum DetTypeFlag {
     NoDet        = 0x000000,
     SCT_brlpos   = 0x000001,
     SCT_brlneg   = 0x000002,
     SCT_ecpos    = 0x000004,
     SCT_ecneg    = 0x000008,
     SCT_all      = 0x00000F,
     Pixel_brlpos = 0x000010,
     Pixel_brlneg = 0x000020,
     Pixel_ecpos  = 0x000040,
     Pixel_ecneg  = 0x000080,
     Pixel_all    = 0x0000F0,
     TRT_brlpos   = 0x000100,
     TRT_brlneg   = 0x000200,
     TRT_ecpos    = 0x000400,
     TRT_ecneg    = 0x000800,
     TRT_all      = 0x000F00
   };
  Q_DECLARE_FLAGS(DetTypeFlags, DetTypeFlag)

  //Project to which surfaces:
  enum InDetProjPartsFlag {
    NoProjections       = 0x000000,
    BarrelPositive      = 0x000001,
    BarrelNegative      = 0x000002,
    BarrelCentral       = 0x000004,
    EndCapOuterPositive = 0x000010,
    EndCapOuterNegative = 0x000020,
    EndCapInnerPositive = 0x000040,
    EndCapInnerNegative = 0x000080,
    //TRT only:
    TRT_EndCapZToRPositive = 0x000100,
    TRT_EndCapZToRNegative = 0x000200,
    TRT_EndCapZToRCentral  = 0x000400,
    //Convenience:
    Barrel_All    = 0x000007,
    Barrel_AllPos = 0x000005,
    Barrel_AllNeg = 0x000006,
    EndCap_All    = 0x0007F0,
    EndCap_AllPos = 0x000550,
    EndCap_AllNeg = 0x0006A0
  };
  Q_DECLARE_FLAGS(InDetProjPartsFlags, InDetProjPartsFlag)

  //To null out parts of the flags, if the corresponding detector parts are not used:
  static void projectionPartsGivenUsedDetectors( InDetProjPartsFlags& proj_pixel,
						 InDetProjPartsFlags& proj_sct,
						 InDetProjPartsFlags& proj_trt,
						 const DetTypeFlags& useddets );

private:
  InDetProjFlags();
  ~InDetProjFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(InDetProjFlags::DetTypeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(InDetProjFlags::InDetProjPartsFlags)

#endif
