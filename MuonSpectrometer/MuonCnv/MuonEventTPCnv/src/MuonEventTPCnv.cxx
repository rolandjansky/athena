/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

//#include "RecTPCnv/MuonContainerCnvARA_p1.h"
#include "MuonEventTPCnv/MuonChamberT0s/ChamberT0sCnv_p1.h"

DECLARE_TPCNV_FACTORY(ChamberT0sCnv_p1,
                      Muon::ChamberT0s,
                      Muon::ChamberT0s_p1,
                      Athena::TPCnvVers::Current)
