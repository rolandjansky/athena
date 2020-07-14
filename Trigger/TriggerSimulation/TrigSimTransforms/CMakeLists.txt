# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSimTransforms )

# Component(s) in the package:
atlas_add_component( TrigSimTransforms
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel EventInfo GaudiKernel SGTools StoreGateLib TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/*.sh )
