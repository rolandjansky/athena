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
find_package( Qt5 COMPONENTS Core OpenGL Gui Widgets )
find_package( SoQt )
find_package( Coin3D )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# get the package name into the variable 'pkgName', to be used below
atlas_get_package_name( pkgName )

# Build the library.
atlas_add_library( ${pkgName} ${pkgName}/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS ${pkgName}
   INCLUDE_DIRS ${SOQT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS} ${QT5_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS tmpqt_extraheaders/  ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets ${SOQT_LIBRARIES} ${COIN3D_LIBRARIES} GeoPrimitives
   PRIVATE_LINK_LIBRARIES VP1GuideLineSystems VP1GeometrySystems VP1AODSystems
)



