/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

//MuonFeature p1
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"
//MuonFeature p2
#include "TrigMuonEventTPCnv/MuonFeature_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_tlp2.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp2.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p2.h"

#include "TrigMuonEventTPCnv/MuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_p3.h"

#include "TrigMuonEventTPCnv/MuonFeatureDetails_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p1.h"

#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p2.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeature_tlp1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_tlp1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p2.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p2.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p4.h"

#include "TrigMuonEventTPCnv/IsoMuonFeature_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p3.h"

#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_p1.h"

#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"

#include "TrigMuonEventTPCnv/TileTrackMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p2.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_tlp1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_p3.h"

#include "TrigMuonEventTPCnv/TrigMuonEFContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p2.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp2.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p6.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p6.h"

#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainerCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainerCnv_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p2.h"

DECLARE_TPCNV_FACTORY(CombinedMuonFeatureCnv_tlp1,
                      CombinedMuonFeature,
                      CombinedMuonFeature_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(CombinedMuonFeatureContainerCnv_tlp1,
                      CombinedMuonFeatureContainer,
                      CombinedMuonFeatureContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CombinedMuonFeatureContainerCnv_p2,
                      CombinedMuonFeatureContainer,
                      CombinedMuonFeatureContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CombinedMuonFeatureContainerCnv_p3,
                      CombinedMuonFeatureContainer,
                      CombinedMuonFeatureContainer_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CombinedMuonFeatureContainerCnv_p4,
                      CombinedMuonFeatureContainer,
                      CombinedMuonFeatureContainer_p4,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(IsoMuonFeatureContainerCnv_tlp1,
                      IsoMuonFeatureContainer,
                      IsoMuonFeatureContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(IsoMuonFeatureContainerCnv_p2,
                      IsoMuonFeatureContainer,
                      IsoMuonFeatureContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(IsoMuonFeatureContainerCnv_p3,
                      IsoMuonFeatureContainer,
                      IsoMuonFeatureContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureCnv_p1,
                      MuonFeature,
                      MuonFeature_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureCnv_p2,
                      MuonFeature,
                      MuonFeature_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureContainerCnv_tlp1,
                      MuonFeatureContainer,
                      MuonFeatureContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureContainerCnv_tlp2,
                      MuonFeatureContainer,
                      MuonFeatureContainer_tlp2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureContainerCnv_p3,
                      MuonFeatureContainer,
                      MuonFeatureContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureDetailsCnv_p1,
                      MuonFeatureDetails,
                      MuonFeatureDetails_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureDetailsCnv_p2,
                      MuonFeatureDetails,
                      MuonFeatureDetails_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureDetailsContainerCnv_tlp1,
                      MuonFeatureDetailsContainer,
                      MuonFeatureDetailsContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonFeatureDetailsContainerCnv_p2,
                      MuonFeatureDetailsContainer,
                      MuonFeatureDetailsContainer_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TileMuFeatureContainerCnv_tlp1,
                      TileMuFeatureContainer,
                      TileMuFeatureContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TileMuFeatureContainerCnv_p2,
                      TileMuFeatureContainer,
                      TileMuFeatureContainer_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TileTrackMuFeatureContainerCnv_tlp1,
                      TileTrackMuFeatureContainer,
                      TileTrackMuFeatureContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TileTrackMuFeatureContainerCnv_p3,
                      TileTrackMuFeatureContainer,
                      TileTrackMuFeatureContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigMuonClusterFeatureContainerCnv_tlp1,
                      TrigMuonClusterFeatureContainer,
                      TrigMuonClusterFeatureContainer_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigMuonEFContainerCnv_tlp1,
                      TrigMuonEFContainer,
                      TrigMuonEFContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigMuonEFContainerCnv_p2,
                      TrigMuonEFContainer,
                      TrigMuonEFContainer_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigMuonEFInfoContainerCnv_tlp1,
                      TrigMuonEFInfoContainer,
                      TrigMuonEFInfoContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigMuonEFInfoContainerCnv_tlp2,
                      TrigMuonEFInfoContainer,
                      TrigMuonEFInfoContainer_tlp2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TrigMuonEFIsolationContainerCnv_p1,
		      TrigMuonEFIsolationContainer,
		      TrigMuonEFIsolationContainer_p1,
		      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigMuonEFIsolationContainerCnv_p2,
		      TrigMuonEFIsolationContainer,
		      TrigMuonEFIsolationContainer_p2,
		      Athena::TPCnvVers::Current)
