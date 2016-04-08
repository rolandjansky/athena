################################################################################
# Package: TrigCombinedEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigCombinedEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigCombinedEvent
                          Trigger/TrigEvent/TrigNavigation
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigCombinedEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigCombinedEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCombinedEvent TrigNavigationLib TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigCombinedEventTPCnvDict
                      TrigCombinedEventTPCnv/TrigCombinedEventTPCnvDict.h
                      TrigCombinedEventTPCnv/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCombinedEvent TrigNavigationLib TestTools AthenaKernel GaudiKernel TrigCombinedEventTPCnv )

atlas_add_test( TrigCompositeCnv_p1_test
                SOURCES
                test/TrigCompositeCnv_p1_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCombinedEvent TrigNavigationLib TestTools AthenaKernel GaudiKernel TrigCombinedEventTPCnv )

