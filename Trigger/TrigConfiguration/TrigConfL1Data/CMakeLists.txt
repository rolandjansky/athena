# $Id: CMakeLists.txt 766241 2016-08-03 14:05:18Z will $
################################################################################
# Package: TrigConfL1Data
################################################################################

# Declare the package name:
atlas_subdir( TrigConfL1Data )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
  Trigger/TrigConfiguration/TrigConfBase )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigConfL1Data
  TrigConfL1Data/*.h Root/*.cxx
  PUBLIC_HEADERS TrigConfL1Data
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES}
  PRIVATE_LINK_LIBRARIES TrigConfBase )

#FIXME: Library for AthAnalysisBase??
