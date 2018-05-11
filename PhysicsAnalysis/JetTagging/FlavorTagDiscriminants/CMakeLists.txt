#####################################
# Flavor tagging discriminant tools
#####################################
#
# This package is a collection of 'duel-use' tools to calculate
# high-level flavor tagging discriminants. Because it should work both
# inside and outside Athena, nothing here can use the magnetic field,
# atlas geometry, or material maps, but neural networks etc are all
# fine.


# Declare the package name:
atlas_subdir( FlavorTagDiscriminants )


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
   Reconstruction/Jet/JetInterface
   Reconstruction/Jet/JetCalibTools
   Reconstruction/Jet/JetRec
   PhysicsAnalysis/Interfaces/JetAnalysisInterfaces
   PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
   PRIVATE
   Reconstruction/Jet/JetEDM
   ${extra_deps}
   #External/Asg_Boost
   )

# External dependencies:
# find_package( FastJet )
# find_package( ROOT COMPONENTS Matrix TMVA )
# find_package( Boost )
# find_package( lwtnn )

# Build a shared library:
atlas_add_library( FlavorTagDiscriminantsLib
  BTagJetAugmenter.cxx
  PUBLIC_HEADERS FlavorTagDiscriminantsLib
   # INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
   # LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES}
   # ${LWTNN_LIBRARIES}
   # AsgTools xAODBase xAODJet JetInterface JetCalibToolsLib 
   # MuonSelectorToolsLib MuonMomentumCorrectionsLib PATCoreLib PATInterfaces 
   # JetAnalysisInterfacesLib InDetTrackSelectionToolLib InDetTrackSystematicsToolsLib 
   # MuonAnalysisInterfacesLib ${extra_libs} 
   # PRIVATE_LINK_LIBRARIES PathResolver JetEDM
   )

if( NOT XAOD_STANDALONE )
   atlas_add_component( FlavorTagDiscriminants
      src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} BoostedJetTaggersLib JetCalibToolsLib)
endif()

# atlas_add_dictionary( BoostedJetTaggersDict
#    BoostedJetTaggers/BoostedJetTaggersDict.h
#    BoostedJetTaggers/selection.xml 
#    LINK_LIBRARIES BoostedJetTaggersLib )

