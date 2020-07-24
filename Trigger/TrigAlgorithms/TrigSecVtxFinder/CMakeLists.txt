# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSecVtxFinder )

# Component(s) in the package:
atlas_add_component( TrigSecVtxFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES BeamSpotConditionsData CxxUtils GaudiKernel GeoPrimitives TrigInDetEvent TrigInDetToolInterfacesLib TrigInterfacesLib TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
