################################################################################
# Package: MuonSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSelectorTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking 
			  Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonSelectorToolsLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS MuonSelectorTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODMuon GaudiKernel PATCoreLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODCore xAODEventInfo xAODTracking PathResolver )

atlas_add_component( MuonSelectorTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODMuon GaudiKernel PATCoreLib AthenaBaseComps xAODCore xAODEventInfo xAODTracking MuonSelectorToolsLib PathResolver )

atlas_add_dictionary( MuonSelectorToolsDict
                      MuonSelectorTools/MuonSelectorToolsDict.h
                      MuonSelectorTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODMuon GaudiKernel PATCoreLib AthenaBaseComps xAODCore xAODEventInfo xAODTracking MuonSelectorToolsLib PathResolver )

# Install files from the package:
atlas_install_joboptions( share/*.py )

