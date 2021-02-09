################################################################################
# Package: DataQualityTools
################################################################################

# Declare the package name:
atlas_subdir( DataQualityTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( 
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Database/ConnectionManagement/AtlasAuthentication
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          ForwardDetectors/ZDC/ZdcEvent
                          ForwardDetectors/ZDC/ZdcIdentifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecEvent
                          MagneticField/MagFieldInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          PhysicsAnalysis/AnalysisCommon/IsolationSelection
                          PhysicsAnalysis/EventTag/TagEvent
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Reconstruction/RecBackground/RecBackgroundEvent
                          Reconstruction/RecoTools/RecoToolInterfaces
                          TileCalorimeter/TileEvent
                          Tools/LWHists
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Trigger/TrigAnalysis/TrigEgammaMatchingTool
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigMuonMatching
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigT1/TrigT1Result 
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          MCTruthClassifier
)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# this line failed automatic conversion in cmt2cmake :
# macro_append cppflags ""  private

# Component(s) in the package:
atlas_add_component( DataQualityTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests xAODCaloEvent xAODEventInfo xAODJet xAODMissingET xAODMuon xAODTracking LUCID_RawEvent ZdcEvent ZdcIdentifier GaudiKernel InDetIdentifier InDetRawData InDetPrepRawData LArRawEvent LArRecEvent MagFieldInterfaces MuonCalibITools MuonIdHelpersLib MuonRDO MuonRecHelperToolsLib TagEvent RecBackgroundEvent RecoToolInterfaces TileEvent LWHists TrkParameters TrkExInterfaces TrkVertexAnalysisUtilsLib TrigDecisionToolLib TrigCaloEvent TrigMuonEvent TrigParticle TrigT1Result MuonSelectorToolsLib TrigAnalysisInterfaces TrigBunchCrossingTool )

# Install files from the package:
#atlas_install_headers( DataQualityTools )
atlas_install_joboptions( share/*.py )
