# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuSuperEF )

# Component(s) in the package:
atlas_add_component( TrigMuSuperEF
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel CaloEvent FourMomUtils GaudiKernel MuidEvent MuonCombinedEvent MuonCombinedToolInterfaces MuonPattern MuonRecToolInterfaces MuonSegment Particle TrigConfHLTData TrigInterfacesLib TrigMuonEvent TrigMuonToolInterfaces TrigNavigationLib TrigSteeringEvent TrigT1Interfaces TrigTimeAlgsLib TrkSegment TrkToolInterfaces xAODMuon xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
atlas_install_joboptions( share/test_TrigMuSuperEF.py share/TrigMuSuperEF_debug_postOptions.py )

