# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileIdentifier )

# Component(s) in the package:
atlas_add_library( TileIdentifier
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileIdentifier
                   LINK_LIBRARIES CaloIdentifier Identifier
                   PRIVATE_LINK_LIBRARIES IdDict GaudiKernel )

atlas_add_dictionary( TileIdentifierDict
                      TileIdentifier/TileIdentifierDict.h
                      TileIdentifier/selection.xml
                      LINK_LIBRARIES TileIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
