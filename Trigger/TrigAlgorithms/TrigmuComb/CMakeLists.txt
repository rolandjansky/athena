# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigmuComb )

# Component(s) in the package:
atlas_add_component( TrigmuComb
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib CxxUtils GaudiKernel MagFieldInterfaces PathResolver StoreGateLib TrigConfHLTData TrigInDetEvent TrigInterfacesLib TrigMuonEvent TrigT1Interfaces TrigTimeAlgsLib TrkExInterfaces xAODTracking xAODTrigMuon MagFieldElements MagFieldConditions )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
