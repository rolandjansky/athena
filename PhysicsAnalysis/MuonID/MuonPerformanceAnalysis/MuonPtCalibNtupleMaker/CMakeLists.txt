################################################################################
# Package: MuonPtCalibNtupleMaker
################################################################################

# Declare the package name:
atlas_subdir( MuonPtCalibNtupleMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DataQuality/GoodRunsLists
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonPtCalibNtupleMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GoodRunsListsLib xAODEventInfo xAODMuon MuonSelectorToolsLib GaudiKernel )

# Install files from the package:
atlas_install_headers( MuonPtCalibNtupleMaker )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

