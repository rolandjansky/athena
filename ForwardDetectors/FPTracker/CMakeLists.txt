################################################################################
# Package: FPTracker
################################################################################

# Declare the package name:
atlas_subdir( FPTracker )

# External dependencies:
find_package( Boost COMPONENTS program_options filesystem thread system )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend fpTrackerMainlinkopts " $(Boost_linkopts_program_options) "

# Component(s) in the package:
atlas_add_library( FPTracker
                   src/*.cxx
                   PUBLIC_HEADERS FPTracker
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_executable( fpTrackerMain
                      test/fpTrackerMain.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${Boost_LIBRARIES} FPTracker )

