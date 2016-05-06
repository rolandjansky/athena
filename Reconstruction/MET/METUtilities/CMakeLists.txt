################################################################################
# Package: METUtilities
################################################################################

# Declare the package name:
atlas_subdir( METUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          Reconstruction/MET/METInterface
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          Event/FourMomUtils
                          Event/xAOD/xAODCore
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          PhysicsAnalysis/TauID/TauAnalysisTools
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
# Component(s) in the package:
atlas_add_library( METUtilitiesLib
                   Root/*.cxx
                   PUBLIC_HEADERS METUtilities
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   LINK_LIBRARIES AsgTools xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTracking GaudiKernel PATInterfaces METInterface
		   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps EventPrimitives FourMomUtils xAODCore PathResolver )

atlas_add_component( METUtilities
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTracking GaudiKernel PATInterfaces METInterface AthenaBaseComps EventPrimitives FourMomUtils xAODCore PathResolver METUtilitiesLib  )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

