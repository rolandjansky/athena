# $Id: CMakeLists.txt 751142 2016-05-31 14:22:21Z krasznaa $
################################################################################
# Package: xAODLuminosity
################################################################################

# Declare the package name:
atlas_subdir( xAODLuminosity )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Event/xAOD/xAODCore
   PRIVATE
   Control/AthLinks )

# Component(s) in the package:
atlas_add_library( xAODLuminosity
   xAODLuminosty/*.h xAODLuminosity/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODLuminosity
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_dictionary( xAODLuminosityDict
   xAODLuminosity/xAODLuminosityDict.h
   xAODLuminosity/selection.xml
   LINK_LIBRARIES AthLinks xAODLuminosity
   EXTRA_FILES Root/dict/*.cxx )

# Generate CLIDs from the library:
atlas_generate_cliddb( xAODLuminosity )
