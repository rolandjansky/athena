################################################################################
# Package: TrigTauRec
################################################################################

# Declare the package name:
atlas_subdir( TrigTauRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          LumiBlock/LumiBlockComps
                          Reconstruction/tauRecTools
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          Calorimeter/CaloEvent
                          Event/EventKernel
                          Event/NavFourMom
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Reconstruction/Particle
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigT1/TrigT1Interfaces 
                          Control/AthAnalysisBaseComps)

# Component(s) in the package:
atlas_add_component( TrigTauRec
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel LumiBlockCompsLib tauRecToolsLib TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloEvent EventKernel NavFourMom xAODJet xAODTau xAODTracking Particle VxVertex TrigT1Interfaces AthAnalysisBaseCompsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Check python syntax:
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )
