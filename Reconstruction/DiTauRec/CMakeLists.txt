################################################################################
# Package: DiTauRec
################################################################################

# Declare the package name:
atlas_subdir( DiTauRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          GaudiKernel
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Reconstruction/Jet/JetEDM
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Calorimeter/CaloEvent
                          Event/xAOD/xAODTracking
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/tauRecTools
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_component( DiTauRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                     LINK_LIBRARIES ${FASTJET_LIBRARIES} AthenaBaseComps xAODEgamma xAODJet xAODMuon xAODTau GaudiKernel MuonSelectorToolsLib JetEDM TrkToolInterfaces CaloEvent xAODTracking RecoToolInterfaces tauRecToolsLib TrkParametersIdentificationHelpers )

# Install files from the package:
atlas_install_headers( DiTauRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )
