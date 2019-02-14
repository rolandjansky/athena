################################################################################
# Package: VP1AODPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1AODPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   graphics/VP1/VP1Base
   PRIVATE
   graphics/VP1/VP1Systems/VP1AODSystems
   graphics/VP1/VP1Systems/VP1GuideLineSystems )

# External dependencies:
find_package( Qt5 COMPONENTS Core )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Build the library.
atlas_add_library( VP1AODPlugin
   VP1AODPlugin/*.h src/*.cxx
   PUBLIC_HEADERS VP1AODPlugin
   LINK_LIBRARIES Qt5::Core VP1Base
   PRIVATE_LINK_LIBRARIES VP1GuideLineSystems VP1AODSystems )
