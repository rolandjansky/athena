# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigMuonEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigMuonEventTPCnv
                         LINK_LIBRARIES AthLinks AthenaPoolCnvSvcLib AthenaPoolUtilities DataModelAthenaPoolLib EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( TrigMuonEventTPCnvDict
                      TrigMuonEventTPCnv/TrigMuonEventTPCnvDict.h
                      TrigMuonEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigMuonEventTPCnv )

atlas_add_dictionary( OLD_TrigMuonEventTPCnvDict
                      TrigMuonEventTPCnv/TrigMuonEventTPCnvDict.h
                      TrigMuonEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES TrigMuonEventTPCnv )

# Tests:
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES GaudiKernel TestTools TrigMuonEventTPCnv )
endfunction()

_add_test( CombinedMuonFeatureCnv_p2_test )
_add_test( CombinedMuonFeatureCnv_p3_test )
_add_test( CombinedMuonFeatureCnv_p4_test )
_add_test( IsoMuonFeatureCnv_p3_test )
_add_test( MuonFeatureCnv_p1_test )
_add_test( MuonFeatureCnv_p2_test )
_add_test( MuonFeatureCnv_p3_test )
_add_test( MuonFeatureDetailsCnv_p1_test )
_add_test( MuonFeatureDetailsCnv_p2_test )
_add_test( TileMuFeatureCnv_p1_test )
_add_test( TileMuFeatureCnv_p2_test )
_add_test( TileTrackMuFeatureCnv_p2_test )
_add_test( TileTrackMuFeatureCnv_p3_test )
_add_test( TrigMuonClusterFeatureCnv_p1_test )
_add_test( TrigMuonEFCbTrackCnv_p5_test )
_add_test( TrigMuonEFCbTrackCnv_p6_test )
_add_test( TrigMuonEFCbTrackCnv_p7_test )
_add_test( TrigMuonEFCnv_p1_test )
_add_test( TrigMuonEFCnv_p2_test )
_add_test( TrigMuonEFInfoCnv_p4_test )
_add_test( TrigMuonEFInfoCnv_p5_test )
_add_test( TrigMuonEFInfoTrackCnv_p1_test )
_add_test( TrigMuonEFIsolationCnv_p1_test )
_add_test( TrigMuonEFIsolationCnv_p2_test )
_add_test( TrigMuonEFTrackCnv_p5_test )
_add_test( TrigMuonEFTrackCnv_p6_test )
