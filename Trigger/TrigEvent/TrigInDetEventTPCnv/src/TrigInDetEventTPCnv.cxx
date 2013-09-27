/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollectionCnv_tlp1.h"

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p2.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p2.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollectionCnv_p2.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp2.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp3.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp3.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp4.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp4.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p3.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p3.h"

#include "TrigInDetEventTPCnv/TrigHisto1D_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainerCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainer_tlp1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainerCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DCnv_p1.h"

#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainerCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainer_tlp1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainerCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"

#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p4.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p3.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1_old_cnv.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p4.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollectionCnv_p4.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p4.h"

#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollectionCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p1.h" 

#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p2.h" 
#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p2.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollectionCnv_p2.h"

#include "TrigInDetEventTPCnv/TrigTrackCounts_p1.h"   
#include "TrigInDetEventTPCnv/TrigTrackCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCollection_p1.h"    
#include "TrigInDetEventTPCnv/TrigTrackCountsCollection_tlp1.h" 
#include "TrigInDetEventTPCnv/TrigTrackCountsCollectionCnv_tlp1.h" 
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p2.h"

#include "TrigInDetEventTPCnv/TrigVertexCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCnv_p1.h"

#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigVertexCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigVertex_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp2.h"
#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_tlp2.h"
#include "TrigInDetEventTPCnv/TrigVertex_p2.h"

DECLARE_TPCNV_FACTORY(TrigHisto1DCnv_p1,
                      TrigHisto1D,
                      TrigHisto1D_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigHisto1DContainerCnv_p1,
                      TrigHisto1DContainer,
                      TrigHisto1DContainer_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigHisto1DContainerCnv_tlp1,
                      TrigHisto1DContainer,
                      TrigHisto1DContainer_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigHisto2DCnv_p1,
                      TrigHisto2D,
                      TrigHisto2D_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigHisto2DContainerCnv_p1,
                      TrigHisto2DContainer,
                      TrigHisto2DContainer_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigHisto2DContainerCnv_tlp1,
                      TrigHisto2DContainer,
                      TrigHisto2DContainer_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigInDetTrackCollectionCnv_tlp1,
                      TrigInDetTrackCollection,
                      TrigInDetTrackCollection_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigInDetTrackCollectionCnv_tlp2,
                      TrigInDetTrackCollection,
                      TrigInDetTrackCollection_tlp2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigInDetTrackCollectionCnv_tlp3,
                      TrigInDetTrackCollection,
                      TrigInDetTrackCollection_tlp3,
                      Athena::TPCnvVers::Old) 
                      
DECLARE_ARATPCNV_FACTORY(TrigInDetTrackCollectionCnvARA_tlp3,
                         TrigInDetTrackCollection,
                         TrigInDetTrackCollection_tlp3,
                         Athena::TPCnvVers::Old) 
                      
DECLARE_TPCNV_FACTORY(TrigInDetTrackCollectionCnv_tlp4,
                      TrigInDetTrackCollection,
                      TrigInDetTrackCollection_tlp4,
                      Athena::TPCnvVers::Current) 
                      
DECLARE_TPCNV_FACTORY(TrigSpacePointCountsCnv_p1,
                      TrigSpacePointCounts,
                      TrigSpacePointCounts_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigSpacePointCountsCnv_p2,
                      TrigSpacePointCounts,
                      TrigSpacePointCounts_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigSpacePointCountsCnv_p3,
                      TrigSpacePointCounts,
                      TrigSpacePointCounts_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigSpacePointCountsCollectionCnv_tlp1,
                      TrigSpacePointCountsCollection,
                      TrigSpacePointCountsCollection_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigSpacePointCountsCollectionCnv_p4,
                      TrigSpacePointCountsCollection,
                      TrigSpacePointCountsCollection_p4,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTauTracksInfoCollectionCnv_tlp1,
                      TrigTauTracksInfoCollection,
                      TrigTauTracksInfoCollection_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigTauTracksInfoCollectionCnv_p2,
                      TrigTauTracksInfoCollection,
                      TrigTauTracksInfoCollection_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTrackCountsCnv_p1,
                      TrigTrackCounts,
                      TrigTrackCounts_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigTrackCountsCnv_p2,
                      TrigTrackCounts,
                      TrigTrackCounts_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTrackCountsCollectionCnv_tlp1,
                      TrigTrackCountsCollection,
                      TrigTrackCountsCollection_tlp1,
                      Athena::TPCnvVers::Current)
                     
DECLARE_TPCNV_FACTORY(TrigVertexCountsCnv_p1,
                      TrigVertexCounts,
                      TrigVertexCounts_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TrigVertexCountsCollectionCnv_tlp1,
                      TrigVertexCountsCollection,
                      TrigVertexCountsCollection_tlp1,
                      Athena::TPCnvVers::Current)
 
DECLARE_TPCNV_FACTORY(TrigTrtHitCountsCnv_p1,
                      TrigTrtHitCounts,
                      TrigTrtHitCounts_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTrtHitCountsCollectionCnv_p1,
                      TrigTrtHitCountsCollection,
                      TrigTrtHitCountsCollection_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigTrtHitCountsCollectionCnv_tlp1,
                      TrigTrtHitCountsCollection,
                      TrigTrtHitCountsCollection_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigTrtHitCountsCollectionCnv_p2,
                      TrigTrtHitCountsCollection,
                      TrigTrtHitCountsCollection_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigVertexCollectionCnv_tlp1,
                      TrigVertexCollection,
                      TrigVertexCollection_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigVertexCollectionCnv_tlp2,
                      TrigVertexCollection,
                      TrigVertexCollection_tlp2,
                      Athena::TPCnvVers::Current)
