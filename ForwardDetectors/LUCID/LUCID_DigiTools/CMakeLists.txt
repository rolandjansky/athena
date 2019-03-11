################################################################################
# Package: LUCID_DigiTools
################################################################################

# Declare the package name:
atlas_subdir( LUCID_DigiTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( LUCID_DigiTools
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_DigiTools
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

