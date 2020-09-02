# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaMuonCombHypo )

# Component(s) in the package:
atlas_add_component( TrigEgammaMuonCombHypo
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CxxUtils GaudiKernel GeoPrimitives ITrackToVertex MuidEvent StoreGateLib TrigInDetEvent TrigInterfacesLib TrigMuonEvent TrigParticle TrigSteeringEvent TrigTopoEvent VxVertex egammaEvent xAODEgamma xAODMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
