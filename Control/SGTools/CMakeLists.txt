################################################################################
# Package: SGTools
################################################################################

# Declare the package name:
atlas_subdir( SGTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CxxUtils
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/SGMon/SGAudCore )

# External dependencies:
find_package( Boost COMPONENTS thread filesystem system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( SGTools
                   src/*.cxx
                   PUBLIC_HEADERS SGTools
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel CxxUtils GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools SGAudCore )

atlas_add_dictionary( SGToolsDict
                      SGTools/SGToolsDict.h
                      SGTools/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( VersionedKey_test
                SOURCES
                test/VersionedKey_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( safe_clid_test
                SOURCES
                test/safe_clid_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( crc64_test
                SOURCES
                test/crc64_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( exceptions_test
                SOURCES
                test/exceptions_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( StringPool_test
                SOURCES
                test/StringPool_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( DataProxy_test
                SOURCES
                test/DataProxy_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( DataStore_test
                SOURCES
                test/DataStore_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( TransientAddress_test
                SOURCES
                test/TransientAddress_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( CurrentEventStore_test
                SOURCES
                test/CurrentEventStore_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

atlas_add_test( SGFolderItem_test
                SOURCES
                test/SGFolderItem_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel TestTools SGAudCore SGTools )

