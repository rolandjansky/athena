# $Id: CMakeLists.txt 760059 2016-07-06 16:13:46Z mnowak $
################################################################################
# Package: TrigInDetEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/AthenaPOOL/RootConversions
   Event/EventCommonTPCnv
   InnerDetector/InDetDetDescr/InDetIdentifier
   Trigger/TrigEvent/TrigCaloEventTPCnv
   Trigger/TrigEvent/TrigInDetEvent
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaKernel
   Control/CxxUtils
   DetectorDescription/IdDictParser
   GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigInDetEventTPCnv
   TrigInDetEventTPCnv/*.h src/*.cxx
   PUBLIC_HEADERS TrigInDetEventTPCnv
   LINK_LIBRARIES StoreGateLib AthenaPoolCnvSvcLib AthenaPoolUtilities
   RootConversions Identifier EventCommonTPCnv FourMom InDetIdentifier
   TrigCaloEventTPCnv TrigInDetEvent
   PRIVATE_LINK_LIBRARIES AthenaKernel CxxUtils IdDictParser )

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
      INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
      LINK_LIBRARIES ${CLHEP_LIBRARIES} TrigInDetEventTPCnv TestTools
      GaudiKernel CxxUtils StoreGateLib InDetIdentifier IdDictParser
      ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

endforeach()
