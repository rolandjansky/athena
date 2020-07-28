# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonHypo )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigMuonHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel LumiBlockData MuonIdHelpersLib MuonRecHelperToolsLib MuonSegment StoreGateLib TrigConfHLTData TrigInDetEvent TrigInterfacesLib TrigMuonEvent TrigMuonToolInterfaces TrigSteeringEvent TrigT1Interfaces TrigT1Result TrkSegment xAODEventInfo xAODMuon xAODTracking xAODTrigMuon xAODTrigger )

atlas_add_test( TrigMuonHypoConfig
                SCRIPT python -m TrigMuonHypo.TrigMuonHypoConfig
                POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py )

