# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBjetHypo )

# Component(s) in the package:
atlas_add_component( TrigBjetHypo
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthViews AthenaBaseComps AthenaMonitoringKernelLib CxxUtils DecisionHandlingLib EventPrimitives TrigCompositeUtilsLib TrigSteeringEvent xAODBTagging xAODJet xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

# Unit tests:
atlas_add_test( TrigBjetBtagHypoTool SCRIPT python -m TrigBjetHypo.TrigBjetBtagHypoTool
   PROPERTIES TIMEOUT 300
   POST_EXEC_SCRIPT nopost.sh )
