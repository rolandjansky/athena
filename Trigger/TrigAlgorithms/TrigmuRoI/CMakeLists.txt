# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigmuRoI )

# Component(s) in the package:
atlas_add_component( TrigmuRoI
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib CxxUtils GaudiKernel TrigInterfacesLib TrigMuonToolInterfaces TrigNavigationLib TrigSteeringEvent TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_python_modules( python/*.py )
