# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigLongLivedParticlesHypo )

# External dependencies:
find_package( ROOT COMPONENTS TMVA Core )

# Component(s) in the package:
atlas_add_component( TrigLongLivedParticlesHypo
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthViews AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib CxxUtils DecisionHandlingLib FourMomUtils GaudiKernel LumiBlockCompsLib LumiBlockData PathResolver StoreGateLib TrigCompositeUtilsLib TrigSteeringEvent xAODJet xAODTracking xAODTrigger BeamSpotConditionsData )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )
