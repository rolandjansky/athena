################################################################################
# Package: JetSubStructureUtils
################################################################################ 

# Declare the package name:
atlas_subdir( JetSubStructureUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   PhysicsAnalysis/MuonID/MuonSelectorTools
   Reconstruction/Jet/JetEDM
   PRIVATE
   Tools/PathResolver )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib COMPONENTS Nsubjettiness EnergyCorrelator )
find_package( ROOT COMPONENTS Matrix Core MathCore Physics )

# Component(s) in the package:
atlas_add_library( JetSubStructureUtils
   JetSubStructureUtils/*.h Root/*.cxx
   PUBLIC_HEADERS JetSubStructureUtils
   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES}
   xAODJet xAODMuon JetEDM
   MuonSelectorToolsLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )

# Install files from the package:
atlas_install_generic( data/*.dat
   DESTINATION XML/JetSubStructureUtils/data
   EXECUTABLE )
atlas_install_data( data/*.dat )
