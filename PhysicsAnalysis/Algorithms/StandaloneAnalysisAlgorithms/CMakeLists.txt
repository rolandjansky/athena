
# Set the name of the package:
atlas_subdir( StandaloneAnalysisAlgorithms )

# Check that this is a standalone release:
if( NOT XAOD_STANDALONE )
   message( WARNING "This package is only meant for standalone releases" )
   return()
endif()

# External(s):
find_package( ROOT COMPONENTS Core RIO )

# Set up the main library:
atlas_add_library( StandaloneAnalysisAlgorithmsLib
   StandaloneAnalysisAlgorithms/*.h Root/*.cxx
   PUBLIC_HEADERS StandaloneAnalysisAlgorithms
   LINK_LIBRARIES xAODRootAccess SystematicsHandlesLib AnaAlgorithmLib
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers EventLoop )

# Set up a dictionary:
atlas_add_dictionary( StandaloneAnalysisAlgorithmsDict
   StandaloneAnalysisAlgorithms/StandaloneAnalysisAlgorithmsDict.h
   StandaloneAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES StandaloneAnalysisAlgorithmsLib )
