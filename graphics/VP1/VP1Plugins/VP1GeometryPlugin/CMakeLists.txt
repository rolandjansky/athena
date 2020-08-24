################################################################################
# Package: VP1GeometryPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1GeometryPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   graphics/VP1/VP1Base
   PRIVATE
   graphics/VP1/VP1Systems/VP1GeometrySystems
   graphics/VP1/VP1Systems/VP1GuideLineSystems )

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

atlas_add_library( VP1GeometryPlugin
   VP1GeometryPlugin/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS VP1GeometryPlugin
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets
   PRIVATE_LINK_LIBRARIES VP1Base VP1GuideLineSystems VP1GeometrySystems )
