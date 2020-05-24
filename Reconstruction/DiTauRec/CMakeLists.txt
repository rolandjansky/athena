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
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
