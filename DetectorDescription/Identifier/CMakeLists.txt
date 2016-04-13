# $Id: CMakeLists.txt 732166 2016-03-24 13:30:37Z krasznaa $
################################################################################
# Package: Identifier
################################################################################

# Declare the package name:
atlas_subdir( Identifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC GaudiKernel )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( Identifier
   src/Identifier.cxx src/Identifier32.cxx
   src/ExpandedIdentifier.cxx src/Range.cxx
   src/Identifiable.cxx src/IdHelper.cxx
   PUBLIC_HEADERS Identifier
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel )
