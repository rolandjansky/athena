################################################################################
# Package: TrigDecisionEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigDecisionEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Trigger/TrigEvent/TrigSteeringEventTPCnv
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/SGTools
                          GaudiKernel
                          Trigger/TrigEvent/TrigDecisionEvent
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigDecisionEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigDecisionEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent )

atlas_add_dictionary( TrigDecisionEventTPCnvDict
                      TrigDecisionEventTPCnv/TrigDecisionEventTPCnvDict.h
                      TrigDecisionEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent TrigDecisionEventTPCnv )

atlas_add_dictionary( OLD_TrigDecisionEventTPCnvDict
                      TrigDecisionEventTPCnv/TrigDecisionEventTPCnvDict.h
                      TrigDecisionEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p1_test
                SOURCES
                test/TrigDecisionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p2_test
                SOURCES
                test/TrigDecisionCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p3_test
                SOURCES
                test/TrigDecisionCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p4_test
                SOURCES
                test/TrigDecisionCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p5_test
                SOURCES
                test/TrigDecisionCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib TrigSteeringEventTPCnv TestTools AthenaKernel SGTools GaudiKernel TrigDecisionEvent TrigSteeringEvent TrigDecisionEventTPCnv )

