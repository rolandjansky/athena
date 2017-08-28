################################################################################
# Package: VP13DCocktailPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP13DCocktailPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          graphics/VP1/VP1Plugins/VP1PlugUtils
                          graphics/VP1/VP1Systems/VP1AODSystems
                          graphics/VP1/VP1Systems/VP1BPhysSystems
                          graphics/VP1/VP1Systems/VP1CaloClusterSystems
                          graphics/VP1/VP1Systems/VP1CaloReadoutSystems
                          graphics/VP1/VP1Systems/VP1CaloSystems
                          graphics/VP1/VP1Systems/VP1GeometrySystems
                          graphics/VP1/VP1Systems/VP1GuideLineSystems
                          graphics/VP1/VP1Systems/VP1PRDSystems
                          graphics/VP1/VP1Systems/VP1RawDataSystems
                          graphics/VP1/VP1Systems/VP1SimHitSystems
                          graphics/VP1/VP1Systems/VP1TrackSystems
                          graphics/VP1/VP1Systems/VP1TrackingGeometrySystems
                          graphics/VP1/VP1Systems/VP1UtilitySystems )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )



# Install files from the package:
atlas_install_headers( VP13DCocktailPlugin )

