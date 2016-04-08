################################################################################
# Package: TrigConfBase
################################################################################

# Declare the package name:
atlas_subdir( TrigConfBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/AtlasDoxygen )

# External dependencies:
find_package( Boost COMPONENTS program_options regex filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrigConfBase
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS TrigConfBase
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_executable( trigconf_msgsvc_test
                      test/trigconf_msgsvc_test.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase )

