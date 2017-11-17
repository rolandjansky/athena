################################################################################
# Build configuration for BoostedJetTaggers
################################################################################

# Declare the name of the package:
atlas_subdir( BoostedJetTaggers )

# Extra dependencies, based on the environment:
set( extra_deps )
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
   set( extra_libs xAODRootAccess )
else()
   set( extra_deps PRIVATE GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC 
   Control/AthToolSupport/AsgTools 
   Event/xAOD/xAODBase
   Event/xAOD/xAODJet 
   PhysicsAnalysis/MuonID/MuonSelectorTools
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections 
   PhysicsAnalysis/AnalysisCommon/PATCore 
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Reconstruction/Jet/JetInterface
   Reconstruction/Jet/JetCalibTools
   Reconstruction/Jet/JetRec
   PhysicsAnalysis/Interfaces/JetAnalysisInterfaces
   PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
   PRIVATE
   Tools/PathResolver
   PhysicsAnalysis/JetMissingEtID/JetSelectorTools
   PhysicsAnalysis/TrackingID/InDetTrackSelectionTool 
   PhysicsAnalysis/TrackingID/InDetTrackSystematicsTools
   Reconstruction/Jet/JetEDM
   ${extra_deps}
   #External/Asg_Boost
   )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Matrix RooFit TMVA )
find_package( Boost )
find_package( lwtnn )

# Build a shared library:
atlas_add_library( BoostedJetTaggersLib
  BoostedJetTaggers/*.h BoostedJetTaggers/lwtnn/*.h Root/*.h Root/*.cxx
  Root/HbbTaggerDNN.cxx
   PUBLIC_HEADERS BoostedJetTaggers
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES}
   ${LWTNN_LIBRARIES}
   AsgTools 
   xAODBase xAODJet JetInterface JetRecLib JetEDM JetSelectorToolsLib JetCalibToolsLib 
   MuonSelectorToolsLib MuonMomentumCorrectionsLib PATCoreLib PATInterfaces 
   JetAnalysisInterfacesLib InDetTrackSelectionToolLib InDetTrackSystematicsToolsLib 
   MuonAnalysisInterfacesLib ${extra_libs} 
   PRIVATE_LINK_LIBRARIES PathResolver JetEDM )

if( NOT XAOD_STANDALONE )
   atlas_add_component( BoostedJetTaggers
      src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} BoostedJetTaggersLib JetCalibToolsLib)
endif()

atlas_add_dictionary( BoostedJetTaggersDict
   BoostedJetTaggers/BoostedJetTaggersDict.h
   BoostedJetTaggers/selection.xml 
   LINK_LIBRARIES BoostedJetTaggersLib )
   
# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( test_SmoothedWZTagger
      util/test_SmoothedWZTagger.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )   
   atlas_add_executable( test_SmoothedTopTagger
      util/test_SmoothedTopTagger.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
   atlas_add_executable( test_BoostedXbbTagger
      util/test_BoostedXbbTagger.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
   atlas_add_executable( test_JetQGTagger
      util/test_JetQGTagger.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
   atlas_add_executable( test_HbbTaggerDNN
      util/test_HbbTaggerDNN.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
endif()

# Install files from the package:
atlas_install_data( share/* )
