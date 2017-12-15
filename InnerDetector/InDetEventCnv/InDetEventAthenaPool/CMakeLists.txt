# $Id: CMakeLists.txt 749562 2016-05-25 04:45:43Z krasznaa $
################################################################################
# Package: InDetEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( InDetEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/Identifier
   Generators/GeneratorObjectsTPCnv
   PRIVATE
   AtlasTest/TestTools
   Control/AthContainers
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/SGTools
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/AtlasSealCLHEP
   GaudiKernel
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetEventCnv/InDetEventTPCnv
   InnerDetector/InDetRawEvent/InDetRawData
   InnerDetector/InDetRawEvent/InDetSimData
   InnerDetector/InDetRecEvent/InDetLowBetaInfo
   InnerDetector/InDetRecEvent/InDetPrepRawData
   Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetEventAthenaPoolPoolCnv
   InDetEventAthenaPool/*.h src/*.h src/*.cxx
   FILES InDetRawData/PixelRDO_Container.h InDetRawData/SCT_RDO_Container.h
   InDetRawData/TRT_RDO_Container.h InDetPrepRawData/TRT_DriftCircleContainer.h
   InDetPrepRawData/PixelClusterContainer.h
   InDetPrepRawData/SCT_ClusterContainer.h
   InDetPrepRawData/PixelGangedClusterAmbiguities.h src/InDetTrack.h
   InDetSimData/InDetSimDataCollection.h
   InDetLowBetaInfo/InDetLowBetaCandidate.h
   InDetLowBetaInfo/InDetLowBetaContainer.h
   TYPES_WITH_NAMESPACE InDet::InDetLowBetaCandidate
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv AthAllocators AthContainers
   AthenaBaseComps AthenaKernel SGTools StoreGateLib AthenaPoolCnvSvcLib
   AthenaPoolUtilities AtlasSealCLHEP GaudiKernel InDetIdentifier
   InDetReadoutGeometry InDetEventTPCnv InDetRawData InDetSimData
   InDetLowBetaInfo InDetPrepRawData TrkTrack )

atlas_add_dictionary( InDetEventAthenaPoolCnvDict
   InDetEventAthenaPool/InDetEventAthenaPoolCnvDict.h
   InDetEventAthenaPool/selection.xml
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv )

# Install files from the package:
atlas_install_headers( InDetEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )
if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( INDETEVENTATHENAPOOL_REFERENCE_TAG
        InDetEventAthenaPoolReference-01-00-00 )
   run_tpcnv_test( InDetEventTPCnv_16.6.2.1 ESD-16.6.2.1
                   REFERENCE_TAG ${INDETEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
