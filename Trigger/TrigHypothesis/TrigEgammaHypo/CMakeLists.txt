################################################################################
# Package: TrigEgammaHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaHypo )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigEgammaHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthViews AthenaBaseComps AthenaMonitoringKernelLib CaloDetDescrLib CaloEvent CaloUtilsLib DecisionHandlingLib EgammaAnalysisInterfacesLib GaudiKernel LumiBlockCompsLib LumiBlockData PATCoreLib RecoToolInterfaces StoreGateLib TrigCaloRecLib TrigCompositeUtilsLib TrigInterfacesLib TrigMissingEtEvent TrigMultiVarHypoLib TrigNavigationLib TrigSteeringEvent TrigT1Interfaces TrigTimeAlgsLib TrkCaloExtension TrkSurfaces VxVertex egammaEvent egammaInterfacesLib xAODBase xAODCaloEvent xAODEgamma xAODEgammaCnvLib xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODTrigger TrigParticle )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

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
