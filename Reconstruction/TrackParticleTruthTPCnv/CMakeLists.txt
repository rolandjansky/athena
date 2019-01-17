################################################################################
# Package: TrackParticleTruthTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleTruthTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Generators/GeneratorObjectsTPCnv
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrackParticleTruthTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrackParticleTruthTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv Particle ParticleTruth TestTools AthenaKernel SGTools StoreGateLib SGtests )

atlas_add_dictionary( TrackParticleTruthTPCnvDict
                      TrackParticleTruthTPCnv/TrackParticleTruthTPCnvDict.h
                      TrackParticleTruthTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv Particle ParticleTruth TestTools AthenaKernel SGTools StoreGateLib SGtests TrackParticleTruthTPCnv )

atlas_add_test( TrackParticleTruthCollectionCnv_p1_test
                SOURCES
                test/TrackParticleTruthCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv Particle ParticleTruth TestTools AthenaKernel SGTools StoreGateLib SGtests TrackParticleTruthTPCnv )

atlas_add_test( TrackParticleTruthCollectionCnv_p2_test
                SOURCES
                test/TrackParticleTruthCollectionCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv Particle ParticleTruth TestTools AthenaKernel SGTools StoreGateLib SGtests TrackParticleTruthTPCnv )

