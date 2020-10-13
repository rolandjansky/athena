# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileIdCnv )

# Component(s) in the package:
atlas_add_component( TileIdCnv
                     src/*.cxx
                     LINK_LIBRARIES DetDescrCnvSvcLib StoreGateLib IdDictDetDescr GaudiKernel TileIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )
