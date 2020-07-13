# Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigInDetEventTPCnv
   TrigInDetEventTPCnv/*.h src/*.cxx
   PUBLIC_HEADERS TrigInDetEventTPCnv
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv InDetIdentifier RootConversions StoreGateLib TrigCaloEventTPCnv TrigInDetEvent
   PRIVATE_LINK_LIBRARIES AthenaKernel IdDictParser )

atlas_add_dictionary( TrigInDetEventTPCnvDict
   TrigInDetEventTPCnv/TrigInDetEventTPCnvDict.h
   TrigInDetEventTPCnv/selection.xml
   LINK_LIBRARIES TrigInDetEventTPCnv )

# Helper variable used in the test(s):
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Set up the test(s) of the package:
foreach( name TrigHisto1DCnv_p1_test TrigHisto2DCnv_p1_test
      TrigInDetTrackCnv_p1_test TrigInDetTrackCnv_p2_test
      TrigInDetTrackCnv_p3_test TrigInDetTrackCnv_p4_test
      TrigInDetTrackCnv_p5_test TrigInDetTrackCollectionCnv_p1_test
      TrigInDetTrackFitParCnv_p1_test TrigInDetTrackFitParCnv_p2_test
      TrigInDetTrackFitParCnv_p3_test TrigSpacePointCountsCnv_p1_test
      TrigSpacePointCountsCnv_p2_test TrigSpacePointCountsCnv_p3_test
      TrigSpacePointCountsCnv_p4_test TrigTauTracksInfoCnv_p1_test
      TrigTauTracksInfoCnv_p2_test TrigTrackCountsCnv_p2_test
      TrigTrtHitCountsCnv_p1_test TrigTrtHitCountsCnv_p2_test
      TrigVertexCnv_p1_test TrigVertexCnv_p2_test TrigVertexCountsCnv_p1_test )

   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES GaudiKernel IdDictParser TestTools TrigInDetEventTPCnv
      ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

endforeach()
