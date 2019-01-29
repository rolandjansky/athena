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
find_package( Qt5 COMPONENTS Core )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Build the library.
atlas_add_library( VP1GeometryPlugin
   VP1GeometryPlugin/*.h src/*.cxx 
   PUBLIC_HEADERS VP1GeometryPlugin
   LINK_LIBRARIES Qt5::Core VP1Base
   PRIVATE_LINK_LIBRARIES VP1GuideLineSystems VP1GeometrySystems )
