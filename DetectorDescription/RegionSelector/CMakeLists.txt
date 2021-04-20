# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RegionSelector )

# Component(s) in the package:
atlas_add_library( RegionSelectorLib
                   src/*.cxx
                   PUBLIC_HEADERS RegionSelector
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel IRegionSelector Identifier RegSelLUT
                   PRIVATE_LINK_LIBRARIES PathResolver )

atlas_add_component( RegionSelector
                     src/components/*.cxx
                     LINK_LIBRARIES RegionSelectorLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
