# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findxml.cmake 711151 2015-11-27 12:00:55Z krasznaa $
#
# Defines:
#
#  XML_FOUND
#  XML_INCLUDE_DIR
#  XML_LIBRARIES
#

# Find the package using CMake's built-in FindLibXml2.cmake module:
if( xml_FIND_QUIETLY )
   find_package( LibXml2 QUIET )
else()
   find_package( LibXml2 )
endif()

# Set the variables expected by the offline code:
if( LIBXML2_FOUND )
   set( XML_FOUND TRUE )
   set( XML_INCLUDE_DIR ${LIBXML2_INCLUDE_DIR} )
   set( XML_INCLUDE_DIRS ${XML_INCLUDE_DIR} )
   set( XML_LIBRARIES ${LIBXML2_LIBRARIES} )
   set( XML_DEFINITIONS ${LIBXML2_DEFINITIONS} )
endif()
