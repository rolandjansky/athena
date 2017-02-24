# $Id: CMakeLists.txt 778093 2016-10-12 17:09:58Z krasznaa $
################################################################################
# Package: TrigConfHLTData
################################################################################

# Declare the package name:
atlas_subdir( TrigConfHLTData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
   Trigger/TrigConfiguration/TrigConfL1Data )

# External dependencies:
find_package( Boost COMPONENTS system thread )

# Component(s) in the package:
atlas_add_library( TrigConfHLTData
   TrigConfHLTData/*.h Root/*.cxx
   PUBLIC_HEADERS TrigConfHLTData
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfL1Data )
