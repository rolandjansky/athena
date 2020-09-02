# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkJetEtMiss )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkJetEtMiss
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthLinks AthenaBaseComps DerivationFrameworkInterfaces FTagAnalysisInterfacesLib GaudiKernel InDetTrackSelectionToolLib JetAnalysisInterfacesLib JetInterface PFlowUtilsLib ParticleJetToolsLib PathResolver StoreGateLib TrackVertexAssociationToolLib TrigAnalysisInterfaces xAODCaloEvent xAODCore xAODEventInfo xAODJet xAODPFlow xAODTracking xAODTrigger xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
