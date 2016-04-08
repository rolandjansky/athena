################################################################################
# Package: TrigL2LongLivedParticles
################################################################################

# Declare the package name:
atlas_subdir( TrigL2LongLivedParticles )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigger
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthContainers
                          Control/CxxUtils
                          Event/FourMomUtils
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/Jet/JetEvent
                          Tools/PathResolver
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TrigL2LongLivedParticles
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} xAODTrigger TrigMuonEvent TrigInterfacesLib TrigT1Interfaces TrigTimeAlgsLib CaloEvent AthContainers CxxUtils FourMomUtils xAODJet xAODTracking GaudiKernel JetEvent PathResolver TrigCaloEvent TrigNavigationLib TrigParticle TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TrigL2LongLivedParticles )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

