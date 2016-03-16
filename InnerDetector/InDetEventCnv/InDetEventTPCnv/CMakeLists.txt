# $Id: CMakeLists.txt 730306 2016-03-16 10:26:06Z krasznaa $
################################################################################
# Package: InDetEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( InDetEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/DataModel
   Control/DataModelAthenaPool
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/Identifier
   GaudiKernel
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetRecEvent/InDetCompetingRIOsOnTrack
   InnerDetector/InDetRecEvent/InDetLowBetaInfo
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   Tracking/TrkEventCnv/TrkEventCnvTools
   Tracking/TrkEventCnv/TrkEventTPCnv
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/CxxUtils
   Control/SGTools
   DetectorDescription/IdDictParser
   Event/EventPrimitives
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_tpcnv_library( InDetEventTPCnv
   InDetEventTPCnv/*.h src/*.cxx
   InDetEventTPCnv/InDetRIO_OnTrack/*.h src/InDetRIO_OnTrack/*.cxx
   InDetEventTPCnv/InDetPrepRawData/*.h src/InDetPrepRawData/*.cxx
   InDetEventTPCnv/InDetCompetingRIOsOnTrack/*.h
   src/InDetCompetingRIOsOnTrack/*.cxx
   InDetEventTPCnv/InDetLowBetaInfo/*.h src/InDetLowBetaInfo/*.cxx
   PUBLIC_HEADERS InDetEventTPCnv
   LINK_LIBRARIES DataModel DataModelAthenaPoolLib StoreGateLib
   AthenaPoolCnvSvcLib AthenaPoolUtilities Identifier GaudiKernel
   InDetIdentifier InDetCompetingRIOsOnTrack InDetLowBetaInfo
   InDetPrepRawData InDetRIO_OnTrack TrkEventTPCnv
   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel CxxUtils
   EventPrimitives InDetReadoutGeometry TrkSurfaces TrkEventPrimitives
   TrkMeasurementBase TrkRIO_OnTrack )

atlas_add_dictionary( InDetEventTPCnvDict
   InDetEventTPCnv/InDetEventTPCnvDict.h InDetEventTPCnv/selection.xml
   LINK_LIBRARIES InDetEventTPCnv )

# A helper variable:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Set up all the tests in the package:
foreach( name SiWidthCnv_p1_test SiWidthCnv_p2_test PixelClusterCnv_p1_test
      PixelClusterCnv_p2_test PixelClusterCnv_p3_test
      PixelClusterContainerCnv_p1_test PixelClusterContainerCnv_p2_test
      PixelClusterContainerCnv_p3_test SCT_ClusterContainerCnv_p2_test
      SCT_ClusterContainerCnv_p3_test TRT_DriftCircleContainerCnv_p2_test
      SCT_ClusterCnv_p1_test SCT_ClusterCnv_p2_test SCT_ClusterCnv_p3_test
      TRT_DriftCircleCnv_p1_test TRT_DriftCircleCnv_p2_test
      PixelGangedClusterAmbiguitiesCnv_p1_test
      InDetLowBetaCandidateCnv_p1_test )

   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES InDetEventTPCnv GaudiKernel IdDictParser CxxUtils
      SGTools InDetReadoutGeometry TestTools InDetIdentifier
      ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

endforeach()
