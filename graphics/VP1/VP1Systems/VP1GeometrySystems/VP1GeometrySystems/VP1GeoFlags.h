/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  enum SubSystemFlag {
    None = 0x000000,
    Pixel = 0x000001,
    SCT = 0x000002,
    TRT = 0x000004,
    InDetServMat = 0x000008,
    LAr = 0x000010,
    Tile = 0x000020,
    BarrelToroid = 0x000040, // FIXME - make barrel
    MuonBarrelStationInner  = 0x000080,
    MuonBarrelStationMiddle = 0x000100,
    MuonBarrelStationOuter  = 0x000200,
    MuonEndcapStationCSC    = 0x000400,
    MuonEndcapStationTGC    = 0x000800,
    MuonEndcapStationMDT    = 0x001000,
    AllMuonChambers         = 0x001F80,
    CavernInfra = 0x002000,
    BeamPipe = 0x004000,
    LUCID = 0x010000,
    ZDC = 0x020000,//,
    ALFA = 0x040000, // bit 18
    ToroidECA = 0x80000, //bit 19
    ToroidECC = 0x100000, //bit 20 FIXME! Remove and cleanup nomenclature
    MuonFeet = 0x200000, //bit 21
    MuonShielding = 0x400000, //bit 22
    MuonToroidsEtc = 0x800000, //bit 23
    MuonNSW = 0x1000000, //bit 24
    ForwardRegion    = 0x2000000, // bit 25
    AllUnrecognisedVolumes = 0x80000000 // bit 31
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
