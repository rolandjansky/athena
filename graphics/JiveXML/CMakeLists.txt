# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JiveXML )

# External package(s).
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
if ( ATLAS_OS_ID STREQUAL "centos9" )
   find_package( RPC )
endif()

# Component(s) in the package:
if ( RPC_FOUND )
  atlas_add_library( JiveXMLLib
   JiveXML/*.h src/*.cxx
   PUBLIC_HEADERS JiveXML
   INCLUDE_DIRS ${RPC_INCLUDE_DIRS}
   LINK_LIBRARIES ${RPC_LIBRARIES} AthenaBaseComps GaudiKernel
   PRIVATE_LINK_LIBRARIES EventInfo xAODEventInfo )
else()
  atlas_add_library( JiveXMLLib
   JiveXML/*.h src/*.cxx
   PUBLIC_HEADERS JiveXML
   INCLUDE_DIRS ${RPC_INCLUDE_DIRS}
   LINK_LIBRARIES AthenaBaseComps GaudiKernel
   PRIVATE_LINK_LIBRARIES EventInfo xAODEventInfo ) 
endif() 

atlas_add_component( JiveXML
                     src/components/*.cxx
                     LINK_LIBRARIES JiveXMLLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml test/*.xsl )
