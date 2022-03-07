# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTrackingHypo )

# External dependencies:
find_package( ROOT COMPONENTS TMVA Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigTrackingHypo
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthViews AthenaBaseComps AthenaMonitoringKernelLib DecisionHandlingLib GaudiKernel TrigCompositeUtilsLib LumiBlockCompsLib xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )
