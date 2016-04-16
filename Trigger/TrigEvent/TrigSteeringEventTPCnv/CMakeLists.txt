################################################################################
# Package: TrigSteeringEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigSteeringEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigSteeringEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigSteeringEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigSteeringEventTPCnvDict
                      TrigSteeringEventTPCnv/TrigSteeringEventTPCnvDict.h
                      TrigSteeringEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_dictionary( OLD_TrigSteeringEventTPCnvDict
                      TrigSteeringEventTPCnv/TrigSteeringEventTPCnvDict.h
                      TrigSteeringEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( HLTResultCnv_p1_test
                SOURCES
                test/HLTResultCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( Lvl1ResultCnv_p2_test
                SOURCES
                test/Lvl1ResultCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( TrigOperationalInfoCnv_p1_test
                SOURCES
                test/TrigOperationalInfoCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( TrigPassBitsCnv_p1_test
                SOURCES
                test/TrigPassBitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( TrigPassFlagsCnv_p1_test
                SOURCES
                test/TrigPassFlagsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( TrigRoiDescriptorCnv_p1_test
                SOURCES
                test/TrigRoiDescriptorCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( TrigRoiDescriptorCnv_p2_test
                SOURCES
                test/TrigRoiDescriptorCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )

atlas_add_test( TrigRoiDescriptorCnv_p3_test
                SOURCES
                test/TrigRoiDescriptorCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent TestTools AthenaKernel GaudiKernel TrigSteeringEventTPCnv )
