# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigStreamerHypo )

# Component(s) in the package:
atlas_add_component( TrigStreamerHypo
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AsgTools AthenaBaseComps DecisionHandlingLib GaudiKernel TrigCompositeUtilsLib TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

# Unit tests:
atlas_add_test( TrigStreamerHypoConfig SCRIPT python -m TrigStreamerHypo.TrigStreamerHypoConfig
                PROPERTIES TIMEOUT 300
		        POST_EXEC_SCRIPT nopost.sh )
