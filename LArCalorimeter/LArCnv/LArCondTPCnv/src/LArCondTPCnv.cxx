/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p1.h"
#include "LArCondTPCnv/LArCaliWaveSubset_p1.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p2.h"
#include "LArCondTPCnv/LArCaliWaveSubset_p2.h"
#include "LArCondTPCnv/LArPhysWaveSubsetCnv_p1.h"
#include "LArCondTPCnv/LArPhysWaveSubset_p1.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"
#include "LArCondTPCnv/LArAutoCorrSubset_p1.h"
#include "LArCondTPCnv/LArOFCSubsetCnv_p1.h"
#include "LArCondTPCnv/LArOFCSubset_p1.h"
#include "LArCondTPCnv/LArPedestalMCCnv_p1.h"
#include "LArCondTPCnv/LArPedestalMC_p1.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"
#include "LArCondTPCnv/LArPedestalSubset_p1.h"
#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"
#include "LArCondTPCnv/LArRampSubset_p1.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p2.h"
#include "LArCondTPCnv/LArShapeSubset_p1.h"
#include "LArCondTPCnv/LArSingleFloatSubsetCnv_p1.h"
#include "LArCondTPCnv/LArSingleFloatSubset_p1.h"
#include "LArCondTPCnv/LArOFCBinSubsetCnv_p1.h"
#include "LArCondTPCnv/LArSingleIntSubset_p1.h"
#include "LArCondTPCnv/LArMphysOverMcalSubset_p1.h"
#include "LArCondTPCnv/LArMphysOverMcalSubsetCnv_p1.h"
#include "LArCondTPCnv/LArDSPThresholdsSubsetCnv_p1.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p2.h"
#include "LArCondTPCnv/LArPedestalSubset_p2.h"

DECLARE_TPCNV_FACTORY(LArAutoCorrSubsetCnv_p1,
                      LArConditionsSubset<LArAutoCorrP1>,
                      LArAutoCorrSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArCaliWaveSubsetCnv_p1,
                      LArConditionsSubset<LArCaliWaveVec>,
                      LArCaliWaveSubset_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArCaliWaveSubsetCnv_p2,
                      LArConditionsSubset<LArCaliWaveVec>,
                      LArCaliWaveSubset_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArDSPThresholdsSubsetCnv_p1,
                      LArConditionsSubset<LArDSPThresholdsP>,
                      LArDSPThresholdsSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArMphysOverMcalSubsetCnv_p1,
                      LArConditionsSubset<LArMphysOverMcalP1>,
                      LArMphysOverMcalSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArOFCBinSubsetCnv_p1,
                      LArConditionsSubset<LArOFCBinP>,
                      LArSingleIntSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArOFCSubsetCnv_p1,
                      LArConditionsSubset<LArOFCP1>,
                      LArOFCSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArPedestalSubsetCnv_p1,
                      LArConditionsSubset<LArPedestalP1>,
                      LArPedestalSubset_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArPedestalSubsetCnv_p2,
                      LArConditionsSubset<LArPedestalP1>,
                      LArPedestalSubset_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArPhysWaveSubsetCnv_p1,
                      LArConditionsSubset<LArPhysWave>,
                      LArPhysWaveSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArRampSubsetCnv_p1,
                      LArConditionsSubset<LArRampP1>,
                      LArRampSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArShapeSubsetCnv_p1,
                      LArConditionsSubset<LArShapeP1>,
                      LArShapeSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArShapeSubsetCnv_p2,
                      LArConditionsSubset<LArShapeP2>,
                      LArShapeSubset_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArSingleFloatSubsetCnv_p1,
                      LArConditionsSubset<LArSingleFloatP>,
                      LArSingleFloatSubset_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArPedestalMCCnv_p1,
                      LArPedestalMC,
                      LArPedestalMC_p1,
                      Athena::TPCnvVers::Current)
