################################################################################
# Package: TrigEFLongLivedParticles
################################################################################

# Declare the package name:
atlas_subdir( TrigEFLongLivedParticles )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Reconstruction/Jet/JetEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          Event/FourMomUtils
                          Event/xAOD/xAODJet
                          GaudiKernel
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_library( TrigEFLongLivedParticlesLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigEFLongLivedParticles
                   LINK_LIBRARIES JetEvent TrigCaloEvent TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES CaloEvent AthenaKernel FourMomUtils xAODJet GaudiKernel TrigParticle )

atlas_add_component( TrigEFLongLivedParticles
                     src/components/*.cxx
                     LINK_LIBRARIES JetEvent TrigCaloEvent TrigSteeringEvent TrigInterfacesLib CaloEvent AthenaKernel FourMomUtils xAODJet GaudiKernel TrigParticle TrigTimeAlgsLib TrigEFLongLivedParticlesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

