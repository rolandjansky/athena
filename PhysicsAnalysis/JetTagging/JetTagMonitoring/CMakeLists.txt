# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTagMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( JetTagMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib AthenaMonitoringLib GaudiKernel ITrackToVertex JetTagToolsLib LArRecEvent LWHists StoreGateLib InDetTrackSelectionToolLib TrkParticleBase TrkTrack TrkVertexFitterInterfaces VxSecVertex VxVertex xAODBTagging xAODEgamma xAODEventInfo xAODJet xAODMuon xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

