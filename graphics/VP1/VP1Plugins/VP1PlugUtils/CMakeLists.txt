################################################################################
# Package: VP1PlugUtils
################################################################################

# Declare the package name:
atlas_subdir( VP1PlugUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          graphics/VP1/VP1Base
                          graphics/VP1/VP1Systems/VP1GeometrySystems
                          graphics/VP1/VP1Systems/VP1GuideLineSystems
                          graphics/VP1/VP1Systems/VP1PRDSystems
                          graphics/VP1/VP1Systems/VP1TrackSystems
                          graphics/VP1/VP1Systems/VP1VertexSystems )

# Component(s) in the package:
atlas_add_library( VP1PlugUtils
                   src/*.c*
                   PUBLIC_HEADERS VP1PlugUtils
                   PRIVATE_LINK_LIBRARIES VP1Base VP1GeometrySystems VP1GuideLineSystems VP1PRDSystems VP1TrackSystems VP1VertexSystems )

