################################################################################
# Package: TrkPatternParameters
################################################################################

# Declare the package name:
atlas_subdir( TrkPatternParameters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParametersBase
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrkPatternParameters
                   src/*.cxx
                   PUBLIC_HEADERS TrkPatternParameters
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES TrkEventPrimitives TrkParametersBase
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel TrkSurfaces )

