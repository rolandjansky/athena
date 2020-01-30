################################################################################
# Package: TrigMonitoringEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigMonitoringEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigMonitoringEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigMonitoringEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigMonitoringEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMonitoringEvent AthenaKernel )

atlas_add_dictionary( TrigMonitoringEventTPCnvDict
                      TrigMonitoringEventTPCnv/TrigMonitoringEventTPCnvDict.h
                      TrigMonitoringEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMonitoringEvent AthenaKernel TrigMonitoringEventTPCnv )


atlas_add_test( TrigConfAlgCnv_p1_test
                SOURCES
                test/TrigConfAlgCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigConfChainCnv_p1_test
                SOURCES
                test/TrigConfChainCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigConfSeqCnv_p1_test
                SOURCES
                test/TrigConfSeqCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigConfSigCnv_p1_test
                SOURCES
                test/TrigConfSigCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonAlgCnv_p1_test
                SOURCES
                test/TrigMonAlgCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonConfigCnv_p1_test
                SOURCES
                test/TrigMonConfigCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonEventCnv_p1_test
                SOURCES
                test/TrigMonEventCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonROBDataCnv_p1_test
                SOURCES
                test/TrigMonROBDataCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonROBDataCnv_p2_test
                SOURCES
                test/TrigMonROBDataCnv_p2_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonROBCnv_p1_test
                SOURCES
                test/TrigMonROBCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonRoiCnv_p1_test
                SOURCES
                test/TrigMonRoiCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonSeqCnv_p1_test
                SOURCES
                test/TrigMonSeqCnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )

atlas_add_test( TrigMonTECnv_p1_test
                SOURCES
                test/TrigMonTECnv_p1_test.cxx
                LINK_LIBRARIES TrigMonitoringEventTPCnv TestTools )              
