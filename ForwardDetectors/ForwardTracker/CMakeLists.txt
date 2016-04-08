################################################################################
# Package: ForwardTracker
################################################################################

# Declare the package name:
atlas_subdir( ForwardTracker )

# External dependencies:
find_package( Boost COMPONENTS program_options filesystem thread system )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend forwardTrackerMainlinkopts " $(Boost_linkopts_program_options) "

# Component(s) in the package:
atlas_add_library( ForwardTracker
                   src/*.cxx
                   PUBLIC_HEADERS ForwardTracker
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_executable( forwardTrackerMain
                      test/forwardTrackerMain.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${Boost_LIBRARIES} ForwardTracker )

