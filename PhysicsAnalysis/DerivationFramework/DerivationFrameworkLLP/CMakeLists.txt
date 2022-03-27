# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkLLP )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkLLP
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthenaBaseComps AthenaKernel AtlasDetDescr CommissionEvent DerivationFrameworkInterfaces ExpressionEvaluationLib FourMomUtils GaudiKernel InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces InDetReadoutGeometry InDetTrackSelectionToolLib InDetConditionsSummaryService LArRecEvent PixelConditionsData SCT_CablingLib SCT_ConditionsToolsLib StoreGateLib TRT_ConditionsServicesLib TRT_ElectronPidToolsLib TRT_ReadoutGeometry TrigDecisionToolLib TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkExInterfaces TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkToolInterfaces TrkTrack TrkVertexFitterInterfaces xAODBase xAODCore xAODEgamma xAODEventInfo xAODJet xAODMuon xAODTau xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
#atlas_install_data( data/* )
