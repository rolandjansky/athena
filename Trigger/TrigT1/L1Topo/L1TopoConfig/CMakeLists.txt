# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoConfig )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( L1TopoConfig
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoConfig
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES TrigConfBase L1TopoCommon )

atlas_add_executable( TrigConfTopoReader
                      src/test/TopoReader.cxx
                      LINK_LIBRARIES L1TopoConfig )

# Install files from the package:
atlas_install_xmls( data/*.xml )

