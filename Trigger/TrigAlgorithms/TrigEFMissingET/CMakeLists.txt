# $Id: CMakeLists.txt 800798 2017-03-16 17:29:50Z jburr $
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
   LArCalorimeter/LArTools
   Trigger/TrigAlgorithms/TrigT2CaloCommon
   Trigger/TrigEvent/TrigCaloEvent
   Trigger/TrigEvent/TrigMissingEtEvent
   Trigger/TrigEvent/TrigParticle
   Trigger/TrigSteer/TrigInterfaces
   Trigger/TrigTools/TrigTimeAlgs
   PRIVATE
   PhysicsAnalysis/AnalysisCommon/PATCore
   Tools/PathResolver
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   Control/CxxUtils
   DetectorDescription/Identifier
   Event/EventKernel
   Event/xAOD/xAODEventInfo
   Reconstruction/Jet/JetEvent
   Trigger/TrigEvent/TrigMuonEvent
   Trigger/TrigEvent/TrigSteeringEvent
   Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib COMPONENTS SoftKiller ConstituentSubtractor )
find_package( ROOT COMPONENTS Core Hist Matrix )
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_library( TrigEFMissingETLib
   TrigEFMissingET/*.h src/*.cxx
   PUBLIC_HEADERS TrigEFMissingET
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier
   AthenaBaseComps xAODCaloEvent xAODJet xAODTrigMissingET GaudiKernel
   LArIdentifier TrigCaloEvent TrigMissingEtEvent TrigParticle CaloDetDescrLib
   LArToolsLib TrigT2CaloCommonLib TrigInterfacesLib TrigTimeAlgsLib
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} CxxUtils Identifier
   EventKernel xAODEventInfo JetEvent TrigMuonEvent TrigSteeringEvent
   PathResolver PATCoreLib
   TrigT1Interfaces )

atlas_add_component( TrigEFMissingET
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel TrigEFMissingETLib ${FASTJET_LIBRARIES}  
   PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES}
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
