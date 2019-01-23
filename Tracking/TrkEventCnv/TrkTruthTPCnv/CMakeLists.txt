################################################################################
# Package: TrkTruthTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          Control/DataModelAthenaPool
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Generators/GeneratorObjects
                          Generators/GeneratorObjectsTPCnv
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/CxxUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrkTruthTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrkTruthTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib GeneratorObjects GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools CxxUtils )

atlas_add_dictionary( TrkTruthTPCnvDict
                      src/TrkTruthTPCnvDict.h
                      TrkTruthTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib GeneratorObjects GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools CxxUtils TrkTruthTPCnv )

atlas_add_test( TrackTruthCollectionCnv_p1_test
                SOURCES
                test/TrackTruthCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools TrkTruthTPCnv )

atlas_add_test( TrackTruthCollectionCnv_p2_test
                SOURCES
                test/TrackTruthCollectionCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools TrkTruthTPCnv )

atlas_add_test( TruthTrajectoryCnv_p1_test
                SOURCES
                test/TruthTrajectoryCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools TrkTruthTPCnv )

atlas_add_test( TruthTrajectoryCnv_p2_test
                SOURCES
                test/TruthTrajectoryCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools TrkTruthTPCnv )

atlas_add_test( PRD_MultiTruthCollectionCnv_p2_test
                SOURCES
                test/PRD_MultiTruthCollectionCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools TrkTruthTPCnv )

atlas_add_test( PRD_MultiTruthCollectionCnv_p3_test
                SOURCES
                test/PRD_MultiTruthCollectionCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData TestTools TrkTruthTPCnv )
