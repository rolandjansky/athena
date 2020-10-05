# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigAFPHypo )

# Component(s) in the package:
atlas_add_component( TrigAFPHypo
                     src/*.cxx src/components/*.cxx
					 LINK_LIBRARIES GaudiKernel TrigParticle TrigSteeringEvent TrigInterfacesLib xAODJet xAODTrigger TrigMissingEtEvent xAODForward PathResolver)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( optics/*.txt )
