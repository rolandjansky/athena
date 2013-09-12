/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigCosmicEventTPCnv/CosmicMuonCnv_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuon_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCollectionCnv_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCollection_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCollectionCnv_tlp1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCollection_tlp1.h"

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCnv_p1.h"  
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCnv_p2.h"  
#include "TrigCosmicEventTPCnv/MdtTrackSegment_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegment_p2.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollectionCnv_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollectionCnv_p2.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollectionCnv_tlp1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p2.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_tlp1.h"

DECLARE_TPCNV_FACTORY(CosmicMuonCnv_p1,
                      CosmicMuon,
                      CosmicMuon_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(CosmicMuonCollectionCnv_tlp1,
                      CosmicMuonCollection,
                      CosmicMuonCollection_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MdtTrackSegmentCnv_p1,
                      MdtTrackSegment,
                      MdtTrackSegment_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(MdtTrackSegmentCnv_p2,
                      MdtTrackSegment,
                      MdtTrackSegment_p2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(MdtTrackSegmentCollectionCnv_tlp1,
                      MdtTrackSegmentCollection,
                      MdtTrackSegmentCollection_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(MdtTrackSegmentCollectionCnv_p2,
                      MdtTrackSegmentCollection,
                      MdtTrackSegmentCollection_p2,
                      Athena::TPCnvVers::Current)
                      
