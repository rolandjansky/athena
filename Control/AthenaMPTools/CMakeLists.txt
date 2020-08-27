# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaMPTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( yampl )

atlas_add_library( AthenaMPToolsLib
                   AthenaMPTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS AthenaMPTools
                   LINK_LIBRARIES AthenaInterprocess GaudiKernel )

# Component(s) in the package:
atlas_add_component( AthenaMPTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${YAMPL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${YAMPL_LIBRARIES} AthenaMPToolsLib AthenaInterprocess GaudiKernel AthenaBaseComps AthenaKernel rt pthread )

