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

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

atlas_add_library( VP1TrackPlugin VP1TrackPlugin/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS VP1TrackPlugin
   INCLUDE_DIRS ${QT5_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS tmpqt_extraheaders/ 
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets
   PRIVATE_LINK_LIBRARIES VP1Base VP1PlugUtils VP1GeometrySystems VP1GuideLineSystems VP1PRDSystems VP1RawDataSystems VP1SimHitSystems VP1TrackSystems VP1VertexSystems
)
