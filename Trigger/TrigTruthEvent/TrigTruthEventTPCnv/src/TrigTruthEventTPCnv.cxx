/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp1.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp2.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp2.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp3.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp3.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old_cnv.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruth_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStats_p1.h"

DECLARE_TPCNV_FACTORY(TrigInDetTrackTruthMapCnv_tlp1,
                      TrigInDetTrackTruthMap,
                      TrigInDetTrackTruthMap_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigInDetTrackTruthMapCnv_tlp2,
                      TrigInDetTrackTruthMap,
                      TrigInDetTrackTruthMap_tlp2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigInDetTrackTruthMapCnv_tlp3,
                      TrigInDetTrackTruthMap,
                      TrigInDetTrackTruthMap_tlp3,
                      Athena::TPCnvVers::Current)
