# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT2HistoPrmVtx )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_component( TrigT2HistoPrmVtx
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps BeamSpotConditionsData CxxUtils EventInfo EventPrimitives GaudiKernel Particle StoreGateLib TrigInDetEvent TrigInterfacesLib TrigNavigationLib TrigSteeringEvent TrigTimeAlgsLib xAODBase xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
