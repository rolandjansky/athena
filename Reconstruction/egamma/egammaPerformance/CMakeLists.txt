# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaPerformance )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

atlas_add_component( egammaPerformance
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringKernelLib AthenaMonitoringLib StoreGateLib xAODEgamma xAODEventInfo GaudiKernel TrigDecisionToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/egammaPerformance_hist.C share/egammaPerformance_ID_hist.C share/egammaPerformanceHistsToPostScript.C share/egammaPerformanceRunMacros.C )

