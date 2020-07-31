# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigmuIso )

# Component(s) in the package:
atlas_add_component( TrigmuIso
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib CxxUtils GaudiKernel StoreGateLib TrigConfHLTData TrigInterfacesLib TrigSteeringEvent TrigT1Interfaces TrigTimeAlgsLib xAODEventInfo xAODTracking xAODTrigMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
