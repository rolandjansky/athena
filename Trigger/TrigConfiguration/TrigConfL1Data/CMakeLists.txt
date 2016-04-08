################################################################################
# Package: TrigConfL1Data
################################################################################

# Declare the package name:
atlas_subdir( TrigConfL1Data )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Trigger/TrigConfiguration/TrigConfBase )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrigConfL1Data
                   Root/*.cxx
                   PUBLIC_HEADERS TrigConfL1Data
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES TrigConfBase )

