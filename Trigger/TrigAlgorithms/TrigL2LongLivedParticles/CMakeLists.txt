# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigL2LongLivedParticles )

# Component(s) in the package:
atlas_add_component( TrigL2LongLivedParticles
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib CxxUtils GaudiKernel StoreGateLib TrigCompositeUtilsLib TrigSteeringEvent xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
