################################################################################
# Package: JetCalibTools
################################################################################

# Declare the package name:
atlas_subdir( JetCalibTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODEventShape
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          Reconstruction/Jet/JetInterface
                          PRIVATE
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread PyROOT )

# Component(s) in the package:
atlas_add_component( JetCalibTools
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODEventShape xAODJet xAODTracking PATInterfaces JetInterface xAODMuon GaudiKernel PathResolver )

# Install files from the package:
atlas_install_headers( JetCalibTools )

