################################################################################
# Package: VP1TrackPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1TrackPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          graphics/VP1/VP1Base
                          graphics/VP1/VP1Plugins/VP1PlugUtils
                          graphics/VP1/VP1Systems/VP1GeometrySystems
                          graphics/VP1/VP1Systems/VP1GuideLineSystems
                          graphics/VP1/VP1Systems/VP1PRDSystems
                          graphics/VP1/VP1Systems/VP1RawDataSystems
                          graphics/VP1/VP1Systems/VP1SimHitSystems
                          graphics/VP1/VP1Systems/VP1TrackSystems
                          graphics/VP1/VP1Systems/VP1VertexSystems )

# Install files from the package:
atlas_install_headers( VP1TrackPlugin )

