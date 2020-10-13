# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODForwardCnv )

# Component(s) in the package:
atlas_add_library( xAODForwardCnvLib
                   xAODForwardCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODForwardCnv
                   LINK_LIBRARIES GaudiKernel TileEvent xAODForward )

atlas_add_component( xAODForwardCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODForward GaudiKernel TileEvent TileIdentifier AthenaBaseComps AthenaKernel xAODForwardCnvLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

