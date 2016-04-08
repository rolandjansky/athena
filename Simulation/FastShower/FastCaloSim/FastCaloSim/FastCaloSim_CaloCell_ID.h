/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastCaloSim_CaloCell_ID_h
#define FastCaloSim_CaloCell_ID_h

#if defined(__CINT__) || defined(__CLING__)
namespace CaloCell_ID {
  enum SUBCALO { LAREM = 0, LARHEC = 1, LARFCAL = 2, TILE = 3, NSUBCALO = 4, NOT_VALID=999999 };

  enum CaloSample {
    PreSamplerB=0, EMB1, EMB2, EMB3,       // LAr barrel
    PreSamplerE, EME1, EME2, EME3,         // LAr EM endcap
    HEC0, HEC1, HEC2, HEC3,                // Hadronic end cap cal.
    TileBar0, TileBar1, TileBar2,      // Tile barrel
    TileGap1, TileGap2, TileGap3,      // Tile gap (ITC & scint)
    TileExt0, TileExt1, TileExt2,      // Tile extended barrel
    FCAL0, FCAL1, FCAL2,                   // Forward EM endcap
    Unknown
  };
}
#else
#include "CaloIdentifier/CaloCell_ID.h"
#endif

namespace CaloCell_ID_FCS {
  enum CaloSample {
    FirstSample=CaloCell_ID::PreSamplerB,
    PreSamplerB=CaloCell_ID::PreSamplerB, EMB1=CaloCell_ID::EMB1, EMB2=CaloCell_ID::EMB2, EMB3=CaloCell_ID::EMB3,       // LAr barrel
    PreSamplerE=CaloCell_ID::PreSamplerE, EME1=CaloCell_ID::EME1, EME2=CaloCell_ID::EME2, EME3=CaloCell_ID::EME3,       // LAr EM endcap
    HEC0=CaloCell_ID::HEC0, HEC1=CaloCell_ID::HEC1, HEC2=CaloCell_ID::HEC2, HEC3=CaloCell_ID::HEC3,      // Hadronic end cap cal.
    TileBar0=CaloCell_ID::TileBar0, TileBar1=CaloCell_ID::TileBar1, TileBar2=CaloCell_ID::TileBar2,      // Tile barrel
    TileGap1=CaloCell_ID::TileGap1, TileGap2=CaloCell_ID::TileGap2, TileGap3=CaloCell_ID::TileGap3,      // Tile gap (ITC & scint)
    TileExt0=CaloCell_ID::TileExt0, TileExt1=CaloCell_ID::TileExt1, TileExt2=CaloCell_ID::TileExt2,      // Tile extended barrel
    FCAL0=CaloCell_ID::FCAL0, FCAL1=CaloCell_ID::FCAL1, FCAL2=CaloCell_ID::FCAL2,                        // Forward EM endcap

    // Beware of MiniFCAL! We don't have it, so different numbers after FCAL2
    
    LastSample = CaloCell_ID::FCAL2,
    MaxSample = LastSample+1,
    noSample = -1
  };
}
#endif

