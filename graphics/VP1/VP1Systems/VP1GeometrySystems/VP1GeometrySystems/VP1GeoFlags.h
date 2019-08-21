/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1GeoFlags                         //
//                                                            //
//  Description: Flags for geometry system.                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1GEOFLAGS_H
#define VP1GEOFLAGS_H

#include <QFlags>
#include <QString>

class VP1GeoFlags {
public:

  enum SubSystemFlag { // 32-bits hexadecimal bitmask
    
    None                    = 0x00000000,
    
    Pixel                   = 0x00000001, // bit 0
    SCT                     = 0x00000002, // 1
    TRT                     = 0x00000004, // 2
    InDetServMat            = 0x00000008, // 3
    
    LAr                     = 0x00000010, // 4
    Tile                    = 0x00000020, // 5
    BarrelToroid            = 0x00000040, // 6 - FIXME - make barrel
    MuonBarrelStationInner  = 0x00000080, // 7
    
    MuonBarrelStationMiddle = 0x00000100, // 8
    MuonBarrelStationOuter  = 0x00000200, // 9
    MuonEndcapStationCSC    = 0x00000400, // 10
    MuonEndcapStationTGC    = 0x00000800, // 11
    
    MuonEndcapStationMDT    = 0x00001000, // 12
    AllMuonChambers         = 0x00001F80, // == all bits occupied by the Muon chambers (80+100+200+400+800+1000)
    CavernInfra             = 0x00002000, // 13
    BeamPipe                = 0x00004000, // 14
    // free                 = 0x00008000, // 15

    LUCID                   = 0x00010000, // 16
    ZDC                     = 0x00020000, // 17
    ALFA                    = 0x00040000, // 18
    ToroidECA               = 0x00080000, // 19
    
    ToroidECC               = 0x00100000, // 20 FIXME! Remove and cleanup nomenclature
    MuonFeet                = 0x00200000, // 21
    MuonShielding           = 0x00400000, // 22
    MuonToroidsEtc          = 0x00800000, // 23
    
    MuonEndcapStationNSW    = 0x01000000, // 24
    ForwardRegion           = 0x02000000, // 25
    // free                 = 0x04000000, // 26
    // free                 = 0x08000000, // 27
    
    // free                 = 0x10000000, // 28
    // free                 = 0x20000000, // 29
    // free                 = 0x40000000, // 30
    AllUnrecognisedVolumes  = 0x80000000  // bit 31
  };
  Q_DECLARE_FLAGS(SubSystemFlags, SubSystemFlag)

  enum MuonChamberAdaptionStyleFlag {
    OpenMDTChambers = 0x000001,
    OpenCSCChambers = 0x000010,
    OpenTGCChambers = 0x000100,
    HideMDTTubes    = 0x001000,
    HideRPCVolumes  = 0x010000
  };
  Q_DECLARE_FLAGS(MuonChamberAdaptionStyleFlags, MuonChamberAdaptionStyleFlag)

  //VolumeState:
  enum VOLSTATE { CONTRACTED,//Show volume if parent is EXPANDED, never show children.
		  EXPANDED,//Dont show volume ( => show all un-zapped children or their children as appropriate).
		  ZAPPED//Dont show volume ( => show no children).
                };
  static QString toString(const VOLSTATE&);

private:
  VP1GeoFlags();
  ~VP1GeoFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(VP1GeoFlags::SubSystemFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(VP1GeoFlags::MuonChamberAdaptionStyleFlags)

#endif
