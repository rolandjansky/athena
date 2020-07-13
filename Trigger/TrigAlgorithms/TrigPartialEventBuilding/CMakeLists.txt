# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name
atlas_subdir( TrigPartialEventBuilding )

# Component(s) in the package
atlas_add_library( TrigPartialEventBuildingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigPartialEventBuilding
                   PRIVATE_LINK_LIBRARIES CxxUtils IRegionSelector
                   LINK_LIBRARIES AthenaBaseComps DecisionHandlingLib TrigCompositeUtilsLib TrigSteeringEvent )

atlas_add_component( TrigPartialEventBuilding
                     src/components/*.cxx
                     LINK_LIBRARIES TrigPartialEventBuildingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
