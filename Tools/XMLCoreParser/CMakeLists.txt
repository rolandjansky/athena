################################################################################
# Package: XMLCoreParser
################################################################################

# Declare the package name:
atlas_subdir( XMLCoreParser )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( EXPAT )

# Component(s) in the package:
atlas_add_library( XMLCoreParser
                   src/XMLCoreParser.cxx
                   src/DOMNode.cxx
                   src/ExpatCoreParser.cxx
                   PUBLIC_HEADERS XMLCoreParser
                   PRIVATE_INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${EXPAT_LIBRARIES} TestTools )

atlas_add_test( example1_test
                SOURCES
                test/example1_test.cxx
                INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                LINK_LIBRARIES ${EXPAT_LIBRARIES} TestTools XMLCoreParser
                ENVIRONMENT "XMLCOREPARSER_DATA=${CMAKE_CURRENT_SOURCE_DIR}/data" )

atlas_add_test( example2_test
                SOURCES
                test/example2_test.cxx
                INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                LINK_LIBRARIES ${EXPAT_LIBRARIES} TestTools XMLCoreParser
                ENVIRONMENT "XMLCOREPARSER_DATA=${CMAKE_CURRENT_SOURCE_DIR}/data" )

atlas_add_executable( example4
                      doc/example4.cxx
                      INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${EXPAT_LIBRARIES} TestTools XMLCoreParser )

atlas_add_executable( texpat
                      src/texpat.cxx
                      src/DOMNode.cxx
                      src/ExpatCoreParser.cxx
                      INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${EXPAT_LIBRARIES} TestTools XMLCoreParser )

