# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP13DCocktailPlugin )

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

# Build the library. // TODO: I removed the TrackCalo plugin so far, because it does not compile with the new plugin system. I should get it back, later.
atlas_add_library( ${pkgName} ${pkgName}/VP13DCocktail*.h src/VP13DCocktail*.cxx src/*.qrc 
   PUBLIC_HEADERS ${pkgName}
   INCLUDE_DIRS ${SOQT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS} ${QT5_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS tmpqt_extraheaders/  ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets ${SOQT_LIBRARIES} ${COIN3D_LIBRARIES} GeoPrimitives
   PRIVATE_LINK_LIBRARIES VP1PlugUtils VP1GuideLineSystems VP1GeometrySystems VP1PRDSystems VP1TrackSystems VP1AODSystems VP1BPhysSystems VP1CaloSystems VP1CaloReadoutSystems VP1UtilitySystems VP1RawDataSystems VP1SimHitSystems VP1TrackingGeometrySystems
)

# Make the package target depend on this one:
#add_dependencies( Package_VP13DCocktailPlugin Package_VP1PlugUtils Package_VP1PRDSystems Package_VP1TrackSystems Package_VP1AODSystems Package_VP1BPhysSystems )
add_dependencies( VP13DCocktailPlugin VP1PlugUtils VP1GeometrySystems VP1GuideLineSystems VP1PRDSystems VP1TrackSystems VP1AODSystems VP1BPhysSystems VP1CaloSystems VP1CaloReadoutSystems VP1UtilitySystems VP1RawDataSystems VP1SimHitSystems VP1TrackingGeometrySystems)

