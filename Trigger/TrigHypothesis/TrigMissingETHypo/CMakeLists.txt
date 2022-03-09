# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMissingETHypo )

# Component(s) in the package:
atlas_add_component( TrigMissingETHypo
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AsgTools AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel TrigCompositeUtilsLib xAODTrigMissingET DecisionHandlingLib)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

# Unit tests:
atlas_add_test( TrigMissingETHypoConfig
                SCRIPT python -m TrigMissingETHypo.TrigMissingETHypoConfig
                PROPERTIES TIMEOUT 300
		        POST_EXEC_SCRIPT nopost.sh )
