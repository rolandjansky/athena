################################################################################
# Package: TrigEgammaHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Calorimeter/CaloUtils
                          Control/AthLinks
                          Control/StoreGate
                          Control/AthenaMonitoringKernel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          LumiBlock/LumiBlockComps
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
                          Reconstruction/egamma/egammaInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/CxxUtils
                          Event/xAOD/xAODEgammaCnv
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaMVACalib
                          Tracking/TrkEvent/TrkCaloExtension
                          Trigger/TrigAlgorithms/TrigCaloRec
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigNavigation 
                          Trigger/TrigSteer/DecisionHandling
                          Control/AthViews
                          Control/AthContainers )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigEgammaHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloUtilsLib AthLinks AthenaBaseComps StoreGateLib SGtests AthenaMonitoringKernelLib xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma GaudiKernel LumiBlockCompsLib PATCoreLib EgammaAnalysisInterfacesLib TrkSurfaces VxVertex TrigCaloEvent TrigInDetEvent TrigParticle TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigTimeAlgsLib CaloEvent CxxUtils ITrackToVertex RecoToolInterfaces egammaEvent egammaMVACalibLib TrkCaloExtension TrigCaloRecLib TrigMissingEtEvent TrigNavigationLib DecisionHandlingLib AthViews )

# Install files from the package:
atlas_install_headers( TrigEgammaHypo )
atlas_install_python_modules( python/*.py )

# Unit tests:
atlas_add_test( TrigL2CaloHypoToolConfig SCRIPT python -m TrigEgammaHypo.TrigL2CaloHypoTool
		POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigL2ElectronHypoToolConfig SCRIPT python -m TrigEgammaHypo.TrigL2ElectronHypoTool
		POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigL2PhotonHypoToolConfig SCRIPT python -m TrigEgammaHypo.TrigL2PhotonHypoTool
		POST_EXEC_SCRIPT nopost.sh )

# Check Python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )
