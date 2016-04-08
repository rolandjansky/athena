################################################################################
# Package: TrigMissingEtEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigMissingEtEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigMissingEtEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigMissingEtEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigMissingEtEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMissingEtEvent TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigMissingEtEventTPCnvDict
                      TrigMissingEtEventTPCnv/TrigMissingEtEventTPCnvDict.h
                      TrigMissingEtEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMissingEtEvent TestTools AthenaKernel GaudiKernel TrigMissingEtEventTPCnv )

atlas_add_test( TrigMissingETCnv_p2_test
                SOURCES
                test/TrigMissingETCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMissingEtEvent TestTools AthenaKernel GaudiKernel TrigMissingEtEventTPCnv )

atlas_add_test( TrigMissingETCnv_p3_test
                SOURCES
                test/TrigMissingETCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMissingEtEvent TestTools AthenaKernel GaudiKernel TrigMissingEtEventTPCnv )

