# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkSeeding )

# External dependencies:
find_package( Acts COMPONENTS Core )

atlas_add_component( ActsTrkSeeding
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES
                     GaudiKernel
                     BeamSpotConditionsData
                     ActsCore
                     ActsTrkEvent
                     ActsTrkToolInterfacesLib
                     MagFieldConditions
                     ActsGeometryInterfacesLib
                     SiSPSeededTrackFinderData
                     )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )



