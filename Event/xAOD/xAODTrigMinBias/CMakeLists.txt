# $Id: CMakeLists.txt 751124 2016-05-31 12:55:34Z krasznaa $
################################################################################
# Package: xAODTrigMinBias
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMinBias )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Event/xAOD/xAODCore
   PRIVATE
   Control/AthLinks )

# Component(s) in the package:
atlas_add_library( xAODTrigMinBias
   xAODTrigMinBias/*.h xAODTrigMinBias/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTrigMinBias
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_dictionary( xAODTrigMinBiasDict
   xAODTrigMinBias/xAODTrigMinBiasDict.h
   xAODTrigMinBias/selection.xml
   LINK_LIBRARIES AthLinks xAODTrigMinBias
   EXTRA_FILES Root/dict/*.cxx )

# Generate CLIDs from the library:
atlas_generate_cliddb( xAODTrigMinBias )
