################################################################################
# Package: TrigTauRec
################################################################################

# Declare the package name:
atlas_subdir( TrigTauRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          LumiBlock/LumiBlockComps
                          Reconstruction/tauRecTools
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloEvent
                          Event/EventKernel
                          Event/NavFourMom
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Reconstruction/Particle
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigT1/TrigT1Interfaces )

# Component(s) in the package:
atlas_add_library( TrigTauRecLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTauRec
                   LINK_LIBRARIES GaudiKernel TrigParticle TrigSteeringEvent LumiBlockCompsLib tauRecToolsLib TrigInterfacesLib TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES CaloEvent EventKernel NavFourMom xAODJet xAODTau xAODTracking Particle VxVertex TrigT1Interfaces )

atlas_add_component( TrigTauRec
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel LumiBlockCompsLib tauRecToolsLib TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloEvent EventKernel NavFourMom xAODJet xAODTau xAODTracking Particle VxVertex TrigT1Interfaces TrigTauRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root )

