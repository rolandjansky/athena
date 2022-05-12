# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaHypo )

# Component(s) in the package:
atlas_add_component( TrigEgammaHypo
                     src/*.cxx
                     src/combos/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ElectronPhotonSelectorToolsLib AthViews AthenaBaseComps AthenaMonitoringKernelLib DecisionHandlingLib EgammaAnalysisInterfacesLib GaudiKernel LumiBlockCompsLib RecoToolInterfaces StoreGateLib TrigCompositeUtilsLib TrigMultiVarHypoLib TrigSteeringEvent TrkCaloExtension xAODBase xAODCaloEvent xAODEgamma xAODEventInfo xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODTrigger AthOnnxruntimeServiceLib RingerSelectorToolsLib)


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

# Unit tests:
atlas_add_test( TrigEgammaFastCaloHypoToolConfig
   SCRIPT python -m TrigEgammaHypo.TrigEgammaFastCaloHypoTool
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigEgammaFastElectronHypoToolConfig
   SCRIPT python -m TrigEgammaHypo.TrigEgammaFastElectronHypoTool
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigEgammaFastPhotonHypoToolConfig
   SCRIPT python -m TrigEgammaHypo.TrigEgammaFastPhotonHypoTool
   POST_EXEC_SCRIPT nopost.sh )
