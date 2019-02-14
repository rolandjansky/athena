// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGMUON_TRIGMUONDEFS_H
#define XAODTRIGMUON_TRIGMUONDEFS_H

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

namespace L2MuonParameters
{

    /// Define chamber types and locations
    enum Chamber {
      BarrelInner  = 0, ///< Inner station in the barrel spectrometer
      BarrelMiddle = 1, ///< Middle station in the barrel spectrometer
      BarrelOuter  = 2, ///< Outer station in the barrel spectrometer
      EndcapInner  = 3, ///< Inner station in the endcap spectrometer
      EndcapMiddle = 4, ///< Middle station in the endcap spectrometer
      EndcapOuter  = 5, ///< Outer station in the endcap spectrometer
      EndcapExtra  = 6, ///< Extra station in the endcap spectrometer
      CSC          = 7, ///< CSC measurement point
      BEE          = 8, ///< BEE measurement point
      BME          = 9, ///< BME measurement point
      Backup       = 10, ///< Backup
      MaxChamber   = 11  ///< Number of measurement point definitions
    };

    ///  Define algoriths ID
	 enum L2MuonAlgoId{GEV900ID=0,   MUONID=1,     HALOID=2,    COSMICID=3,
                      LOOSE_HM=10,  MEDIUM_HM=11, TIGHT_HM=12, LOOSE_LM=13,
                      MEDIUM_LM=14, TIGHT_LM=15,
                      NULLID=99999};

    enum ECRegions{ Bulk, WeakBFieldA, WeakBFieldB};

    ECRegions whichECRegion( const float eta, const float phi );
}

}

#endif // XAODTRIGMUON_TRIGMUONDEFS_H
