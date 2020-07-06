# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBphysMonitoring )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( TrigBphysMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib AthenaMonitoringLib BeamSpotConditionsData GaudiKernel TrigDecisionToolLib TrigHLTMonitoringLib TrkParameters TrkParametersBase TrkVertexAnalysisUtilsLib VxVertex xAODEventInfo xAODMuon xAODTracking xAODTrigBphys xAODTrigMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

