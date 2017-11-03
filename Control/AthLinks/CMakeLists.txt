################################################################################
# Package: AthLinks
################################################################################

# Declare the package name:
atlas_subdir( AthLinks )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthLinks
                   src/*.cxx
                   PUBLIC_HEADERS AthLinks
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( AthLinksDict
                      AthLinks/AthLinksDict.h
                      AthLinks/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( exceptions_test
                SOURCES
                test/exceptions_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( DataProxyHolder_test
                SOURCES
                test/DataProxyHolder_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( IndexHolder_test
                SOURCES
                test/IndexHolder_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( IdentContIndex_test
                SOURCES
                test/IdentContIndex_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( ForwardIndexingPolicy_test
                SOURCES
                test/ForwardIndexingPolicy_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( SetIndexingPolicy_test
                SOURCES
                test/SetIndexingPolicy_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( MapIndexingPolicy_test
                SOURCES
                test/MapIndexingPolicy_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( IdentContIndexingPolicy_test
                SOURCES
                test/IdentContIndexingPolicy_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( IsSTLSequence_test
                SOURCES
                test/IsSTLSequence_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( DataLinkBase_test
                SOURCES
                test/DataLinkBase_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( DataLink_test
                SOURCES
                test/DataLink_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( ElementLinkBase_test
                SOURCES
                test/ElementLinkBase_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( GenericElementLinkBase_test
                SOURCES
                test/GenericElementLinkBase_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( ElementLink_test
                SOURCES
                test/ElementLink_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( ElementLinkFwd_test
                SOURCES
                test/ElementLinkFwd_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks )

atlas_add_test( AssociationMap_test
                SOURCES
                test/AssociationMap_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks
                EXTRA_PATTERNS "ClassIDSvc Initialized successfully|^HistogramPersis.* INFO" )


atlas_add_test( CachedValue_test
                SOURCES
                test/CachedValue_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel TestTools AthLinks
                PROPERTIES TIMEOUT 300
                )
              

