# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( XMLCoreParser )

# External dependencies:
find_package( EXPAT )

# Component(s) in the package:
atlas_add_library( XMLCoreParser
                   src/XMLCoreParser.cxx
                   src/DOMNode.cxx
                   src/ExpatCoreParser.cxx
                   PUBLIC_HEADERS XMLCoreParser
                   PRIVATE_INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${EXPAT_LIBRARIES} CxxUtils )

atlas_add_test( example1_test
                SOURCES
                test/example1_test.cxx
                INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                LINK_LIBRARIES ${EXPAT_LIBRARIES} XMLCoreParser
                ENVIRONMENT "XMLCOREPARSER_DATA=${CMAKE_CURRENT_SOURCE_DIR}/data" )

atlas_add_test( example2_test
                SOURCES
                test/example2_test.cxx
                INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                LINK_LIBRARIES ${EXPAT_LIBRARIES} XMLCoreParser
                ENVIRONMENT "XMLCOREPARSER_DATA=${CMAKE_CURRENT_SOURCE_DIR}/data" )

atlas_add_executable( texpat
                      src/texpat.cxx
                      src/DOMNode.cxx
                      src/ExpatCoreParser.cxx
                      INCLUDE_DIRS ${EXPAT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${EXPAT_LIBRARIES} XMLCoreParser CxxUtils )
