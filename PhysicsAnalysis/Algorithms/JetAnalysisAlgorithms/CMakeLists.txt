# The name of the package:
atlas_subdir( JetAnalysisAlgorithms )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   Control/AthToolSupport/AsgTools )

atlas_add_library( JetAnalysisAlgorithmsLib
   JetAnalysisAlgorithms/*.h JetAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS JetAnalysisAlgorithms
   LINK_LIBRARIES SystematicsHandlesLib JetCalibToolsLib JetInterface JetResolutionLib JetCPInterfaces JetJvtEfficiencyLib
   PRIVATE_LINK_LIBRARIES RootCoreUtils )

atlas_add_dictionary( JetAnalysisAlgorithmsDict
   JetAnalysisAlgorithms/JetAnalysisAlgorithmsDict.h
   JetAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES JetAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel JetAnalysisAlgorithmsLib )
endif()
