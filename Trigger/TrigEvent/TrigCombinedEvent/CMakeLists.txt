################################################################################
# Package: TrigCombinedEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigCombinedEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          GaudiKernel
                          Trigger/TrigEvent/TrigNavigation
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          Control/AthenaKernel
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigCombinedEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigCombinedEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} DataModel SGTools GaudiKernel TrigNavigationLib TrigSerializeCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthenaKernel )

atlas_add_sercnv_library( TrigCombinedEventSerCnv
                          FILES TrigCombinedEvent/TrigComposite.h TrigCombinedEvent/TrigCompositeContainer.h
                          INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel TrigNavigationLib TestTools AthenaKernel TrigSerializeCnvSvcLib TrigCombinedEvent )

atlas_add_dictionary( TrigCombinedEventDict
                      TrigCombinedEvent/TrigCombinedEventDict.h
                      TrigCombinedEvent/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel TrigNavigationLib TestTools AthenaKernel TrigSerializeCnvSvcLib TrigCombinedEvent
                      DATA_LINKS TrigComposite
                      ELEMENT_LINKS TrigCompositeContainer )

atlas_add_test( TrigComposite_test
                SOURCES
                test/TrigComposite_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel TrigNavigationLib TestTools AthenaKernel TrigSerializeCnvSvcLib TrigCombinedEvent
                EXTRA_PATTERNS ".*" )

