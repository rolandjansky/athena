################################################################################
# Package: TrigEFMissingET
################################################################################

# Declare the package name:
atlas_subdir( TrigEFMissingET )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloDetDescr
   Calorimeter/CaloEvent
   Calorimeter/CaloGeoHelpers
   Calorimeter/CaloIdentifier
   Calorimeter/CaloInterface
   Control/AthenaBaseComps
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODJet
   Event/xAOD/xAODTrigMissingET
   Event/FourMomUtils
   GaudiKernel
   LArCalorimeter/LArIdentifier
   Trigger/TrigAlgorithms/TrigT2CaloCommon
   Trigger/TrigEvent/TrigCaloEvent
   Trigger/TrigEvent/TrigMissingEtEvent
   Trigger/TrigEvent/TrigParticle
   Trigger/TrigSteer/TrigInterfaces
   Trigger/TrigTools/TrigTimeAlgs
   PRIVATE
   Tools/PathResolver
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   Control/CxxUtils
   DetectorDescription/Identifier
   Event/EventKernel
   Event/xAOD/xAODEventInfo
   Reconstruction/Jet/JetEvent
   Reconstruction/Jet/JetEDM
   Trigger/TrigEvent/TrigMuonEvent
   Trigger/TrigEvent/TrigSteeringEvent
   Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib COMPONENTS SoftKiller ConstituentSubtractor )
find_package( ROOT COMPONENTS Core Hist Matrix )
find_package( tdaq-common COMPONENTS eformat )
find_package( Eigen )

atlas_add_library( TrigEFMissingETLib
  src/METComponent.cxx
  src/StatusFlags.cxx
  src/SignedKinematics.cxx
  src/PufitGrid.cxx
  src/PufitUtils.cxx
  PUBLIC_HEADERS TrigEFMissingET
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
  LINK_LIBRARIES xAODTrigMissingET xAODBase
)

# Component(s) in the package:
atlas_add_component( TrigEFMissingET
  src/EFMissingET.cxx
  src/EFMissingETHelper.cxx
  src/EFMissingETBaseTool.cxx
  src/EFMissingETComponentCopier.cxx
  src/EFMissingETFromCells.cxx
  src/EFMissingETFromClusters.cxx
  src/EFMissingETFromClustersPS.cxx
  src/EFMissingETFromClustersPUC.cxx
  src/EFMissingETFromFEBHeader.cxx
  src/EFMissingETFromJets.cxx
  src/EFMissingETFromTrackAndJets.cxx
  src/EFMissingETFromClustersTracksPUC.cxx
  src/EFMissingETFromTrackAndClusters.cxx
  src/EFMissingETFlags.cxx
  src/EFMissingETFromHelper.cxx
  src/EFMissingETAlgMT.cxx
  src/EFMissingETFlagsMT.cxx
  src/FexBase.cxx
  src/MonGroupBuilder.cxx
  src/TrkMHTFex.cxx
  src/CellFex.cxx
  src/MHTFex.cxx
  src/TCFex.cxx
  src/TCPufitFex.cxx
  src/components/TrigEFMissingET_entries.cxx
  src/components/TrigEFMissingET_load.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES}
  TrigEFMissingETLib
  AthenaBaseComps CaloDetDescrLib CaloEvent CaloGeoHelpers
  CaloIdentifier CxxUtils EventKernel GaudiKernel Identifier
  JetEvent LArIdentifier PathResolver JetEDM
  TrigCaloEvent TrigInterfacesLib TrigMissingEtEvent TrigMuonEvent
  TrigParticle TrigSteeringEvent TrigT1Interfaces TrigT2CaloCommonLib
  TrigTimeAlgsLib xAODCaloEvent xAODEventInfo xAODJet xAODTrigMissingET )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
