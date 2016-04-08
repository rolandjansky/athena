################################################################################
# Package: eflowEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( eflowEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/eflowEvent
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthLinks
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/Navigation
                          Control/SGTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( eflowEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS eflowEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools )

atlas_add_dictionary( eflowEventTPCnvDict
                      eflowEventTPCnv/eflowEventTPCnvDict.h
                      eflowEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectCnv_p1_test
                SOURCES
                test/eflowObjectCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectCnv_p2_test
                SOURCES
                test/eflowObjectCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectCnv_p3_test
                SOURCES
                test/eflowObjectCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectCnv_p4_test
                SOURCES
                test/eflowObjectCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectCnv_p5_test
                SOURCES
                test/eflowObjectCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectContainerCnv_p1_test
                SOURCES
                test/eflowObjectContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectContainerCnv_p2_test
                SOURCES
                test/eflowObjectContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectContainerCnv_p3_test
                SOURCES
                test/eflowObjectContainerCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectContainerCnv_p4_test
                SOURCES
                test/eflowObjectContainerCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

atlas_add_test( eflowObjectContainerCnv_p5_test
                SOURCES
                test/eflowObjectContainerCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent DataModelAthenaPoolLib AthenaPoolCnvSvcLib muonEvent Particle eflowEvent VxVertex TestTools AthLinks AthenaKernel CxxUtils Navigation SGTools eflowEventTPCnv )

