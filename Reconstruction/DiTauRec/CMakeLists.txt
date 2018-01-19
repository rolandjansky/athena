################################################################################
# Package: DiTauRec
################################################################################

# Declare the package name:
atlas_subdir( DiTauRec )

# Declare the package's dependencies:
if( NOT XAOD_STANDALONE )
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
  

else()
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
endif()

# External dependencies:
find_package( FastJet )

if( XAOD_STANDALONE)
atlas_add_root_dictionary( DiTauRec DiTauRecLibCintDict
  ROOT_HEADERS
  DiTauRec/ILepHadBuilder.h
  DiTauRec/HadElBuilder.h
  DiTauRec/HadMuBuilder.h
  Root/LinkDef.h
  EXTERNAL_PACKAGES ROOT
  )
endif()

# Component(s) in the package:

if( NOT XAOD_STANDALONE )
  atlas_add_component( DiTauRec
    src/*.cxx
    src/components/*.cxx
    INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
    LINK_LIBRARIES ${FASTJET_LIBRARIES} AthenaBaseComps xAODEgamma xAODJet xAODMuon xAODTau GaudiKernel MuonSelectorToolsLib JetEDM TrkToolInterfaces CaloEvent xAODTracking RecoToolInterfaces tauRecToolsLib TrkParametersIdentificationHelpers )
else()
atlas_add_library( DiTauRec
  DiTauRec/*.h Root/*.cxx ${DiTauRecLibCintDict}
  PUBLIC_HEADERS DiTauRec
  LINK_LIBRARIES AsgTools xAODEgamma xAODJet xAODMuon xAODTau MuonSelectorToolsLib JetEDM xAODTracking RecoToolInterfaces tauRecToolsLib
  )
endif()

# Install files from the package:
atlas_install_headers( DiTauRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

