# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTauHypo )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( TrigTauHypo
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthViews AthenaBaseComps AthenaMonitoringKernelLib CxxUtils DecisionHandlingLib GaudiKernel MVAUtils PathResolver TrigCompositeUtilsLib TrigSteeringEvent TrkTrack TrkTrackSummary tauRecToolsLib xAODBase xAODCaloEvent xAODJet xAODTau xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )
