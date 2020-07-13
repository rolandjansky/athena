# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHIHypo )

# Component(s) in the package:
atlas_add_component( TrigHIHypo
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel CaloEvent CaloInterfaceLib GaudiKernel HIGlobalLib Particle StoreGateLib TrigCaloEvent TrigInDetEvent TrigInterfacesLib TrigMissingEtEvent TrigSteeringEvent TrigT1Interfaces TrigTimeAlgsLib TrkParameters xAODHIEvent xAODMuon xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
