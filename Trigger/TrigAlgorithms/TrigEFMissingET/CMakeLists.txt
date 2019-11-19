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
   Trigger/TrigEvent/TrigMuonEvent
   Trigger/TrigEvent/TrigSteeringEvent
   Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib COMPONENTS SoftKiller ConstituentSubtractor )
find_package( ROOT COMPONENTS Core Hist Matrix )
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_component( TrigEFMissingET
   src/*.cxx
   src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES}
   AthenaBaseComps CaloDetDescrLib CaloEvent CaloGeoHelpers
   CaloIdentifier CxxUtils EventKernel GaudiKernel Identifier
   JetEvent LArIdentifier PathResolver
   TrigCaloEvent TrigInterfacesLib TrigMissingEtEvent TrigMuonEvent
   TrigParticle TrigSteeringEvent TrigT1Interfaces TrigT2CaloCommonLib
   TrigTimeAlgsLib xAODCaloEvent xAODEventInfo xAODJet xAODTrigMissingET )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
