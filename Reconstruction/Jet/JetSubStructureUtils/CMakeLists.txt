################################################################################
# Package: JetSubStructureUtils
################################################################################

# Declare the package name:
atlas_subdir( JetSubStructureUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Reconstruction/Jet/JetEDM
                          PRIVATE
                          Tools/PathResolver )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib )
find_package( ROOT COMPONENTS Matrix RooFit Core Tree MathCore Hist RIO pthread )
find_package( ShowerDeconstruction )

# Component(s) in the package:
atlas_add_library( JetSubStructureUtils
                   Root/*.cxx
                   PUBLIC_HEADERS JetSubStructureUtils
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS} ${SHOWERDECONSTRUCTION_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} ${SHOWERDECONSTRUCTION_LIBRARIES} xAODJet xAODMuon JetEDM MuonSelectorToolsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )

# Install files from the package:
atlas_install_generic( data/*.dat
                       DESTINATION XML/JetSubStructureUtils/data
                       EXECUTABLE )

