# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkInDet )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkInDet
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthenaBaseComps AthenaKernel AtlasDetDescr CommissionEvent DerivationFrameworkInterfaces ExpressionEvaluationLib FourMomUtils GaudiKernel InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetReadoutGeometry InDetTrackSelectionToolLib LArRecEvent SCT_CablingLib SCT_ConditionsToolsLib StoreGateLib TRT_ConditionsServicesLib TRT_ElectronPidToolsLib TRT_ReadoutGeometry TrigDecisionToolLib TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkExInterfaces TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkToolInterfaces TrkTrack TrkVertexFitterInterfaces xAODBase xAODCore xAODEgamma xAODEventInfo xAODJet xAODMuon xAODTau xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
