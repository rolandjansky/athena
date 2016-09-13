
################################################################################
# Package: TrigCaloHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloInterface
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Event/EventKernel
                          Event/FourMomUtils
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArCellRec
                          LArCalorimeter/LArRecEvent
                          Reconstruction/Jet/JetUtils
                          TDAQCPolicy )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common COMPONENTS hltinterface )

# Component(s) in the package:
atlas_add_library( TrigCaloHypoLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigJetHypo
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GaudiKernel JetEvent TrigCaloEvent TrigParticle TrigSteeringEvent ${TDAQ-COMMON_LIBRARIES} TrigInterfacesLib TrigTimeAlgsLib 
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloGeoHelpers EventKernel FourMomUtils xAODCaloEvent xAODEventInfo LArRecEvent JetUtils )

atlas_add_component( TrigCaloHypo
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} xAODJet GaudiKernel JetEvent TrigCaloEvent TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloGeoHelpers EventKernel FourMomUtils xAODCaloEvent xAODEventInfo LArRecEvent JetUtils TrigCaloHypoLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/jobOfragment_*.py share/TriggerConfig_*.py )
