# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSteeringTest )

# Component(s) in the package:
atlas_add_component( TrigSteeringTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrigSteeringEvent TrigInterfacesLib AthenaKernel AthContainers xAODTrigger TrigNavigationLib TrigT1Interfaces TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

