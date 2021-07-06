################################################################################
# Build configuration for BoostedJetTaggers
################################################################################

# Declare the name of the package:
atlas_subdir( BoostedJetTaggers )

# Extra dependencies, based on the environment:
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
   set( extra_libs xAODRootAccess )
else()
   set( extra_deps PRIVATE GaudiKernel )
endif()

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Matrix TMVA )
find_package( Boost )
find_package( lwtnn )

# Build a shared library:
atlas_add_library( BoostedJetTaggersLib
  BoostedJetTaggers/*.h
  Root/JSSTaggerBase.cxx
  Root/JetQGTagger.cxx
  Root/JetQGTaggerBDT.cxx
  Root/JSSWTopTaggerDNN.cxx
  Root/JSSWTopTaggerANN.cxx
  Root/SmoothedWZTagger.cxx
   PUBLIC_HEADERS BoostedJetTaggers
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES}
   ${LWTNN_LIBRARIES}
   AsgTools 
   xAODBase xAODJet xAODTruth xAODTracking JetInterface JetRecLib JetEDM JetSelectorToolsLib JetCalibToolsLib  JetUncertaintiesLib ParticleJetToolsLib PathResolver
   MuonSelectorToolsLib MuonMomentumCorrectionsLib PATCoreLib PATInterfaces FlavorTagDiscriminants
   JetAnalysisInterfacesLib InDetTrackSelectionToolLib InDetTrackSystematicsToolsLib 
   MuonAnalysisInterfacesLib ${extra_libs} 
   PRIVATE_LINK_LIBRARIES JetEDM )

if( NOT XAOD_STANDALONE )
   atlas_add_component( BoostedJetTaggers
      src/components/*.cxx
      LINK_LIBRARIES BoostedJetTaggersLib JetCalibToolsLib)
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
      xAODEventInfo xAODJet xAODTruth xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )   
   atlas_add_executable( test_JetQGTagger
      util/test_JetQGTagger.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
   atlas_add_executable( test_JSSWTopTaggerDNN
      util/test_JSSWTopTaggerDNN.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODTruth xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
   atlas_add_executable( test_JSSWTopTaggerANN
      util/test_JSSWTopTaggerANN.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODTruth xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
   atlas_add_executable( test_JetQGTaggerBDT
      util/test_JetQGTaggerBDT.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces xAODCore AsgTools
      BoostedJetTaggersLib
   )
endif()

