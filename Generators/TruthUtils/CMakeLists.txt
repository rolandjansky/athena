# $Id: CMakeLists.txt 744476 2016-05-03 13:24:11Z krasznaa $
################################################################################
# Package: TruthUtils
################################################################################

# Declare the package name:
atlas_subdir( TruthUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/AtlasHepMC )

# External dependencies:
find_package( Boost )
find_package( HEPUtils )
find_package( MCUtils )
find_package( HepMC )

# Extra include directories and libraries, based on which externals were found:
set( extra_includes )
set( extra_libs )
if( HEPUTILS_FOUND )
   list( APPEND extra_includes ${HEPUTILS_INCLUDE_DIRS} )
   list( APPEND extra_libs ${HEPUTILS_LIBRARIES} )
endif()
if( MCUTILS_FOUND )
   list( APPEND extra_includes ${MCUTILS_INCLUDE_DIRS} )
   list( APPEND extra_libs ${MCUTILS_LIBRARIES} )
endif()
if( HEPMC_FOUND )
   list( APPEND extra_includes  )
   list( APPEND extra_libs AtlasHepMCLib )
endif()

# Component(s) in the package:
atlas_add_library( TruthUtils
   TruthUtils/*.h Root/*.cxx
   PUBLIC_HEADERS TruthUtils
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${extra_includes}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${extra_libs} )
