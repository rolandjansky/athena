# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigIDTPMonitor )

# Component(s) in the package:
atlas_add_component( TrigIDTPMonitor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks EventInfo GaudiKernel TrigBphysicsEvent TrigConfHLTData TrigInterfacesLib TrigParticle TrigSteeringEvent xAODCaloEvent xAODEgamma xAODEventInfo xAODMuon xAODTracking xAODTrigBphys )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
