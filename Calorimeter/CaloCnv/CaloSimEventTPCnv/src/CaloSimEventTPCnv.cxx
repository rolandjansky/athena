/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "CaloSimEventTPCnv/CaloCalibrationHitContainerCnv_p3.h"

DECLARE_TPCNV_FACTORY(CaloCalibrationHitContainerCnv_p3,
                      CaloCalibrationHitContainer,
                      CaloCalibrationHitContainer_p3,
                      Athena::TPCnvVers::Current)
