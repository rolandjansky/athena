
################################################################################
# Package: TrigCaloHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloInterface
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          Calorimeter/CaloGeoHelpers
                          Event/EventKernel
                          Event/FourMomUtils
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArRecConditions
                          Reconstruction/Jet/JetUtils )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common COMPONENTS hltinterface )

# Component(s) in the package:
atlas_add_component( TrigCaloHypo
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} xAODJet GaudiKernel JetEvent TrigCaloEvent TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloGeoHelpers EventKernel FourMomUtils xAODCaloEvent xAODEventInfo LArRecEvent LArRecConditions JetUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
