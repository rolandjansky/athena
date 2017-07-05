# $Id: CMakeLists.txt 728676 2016-03-09 15:11:49Z krasznaa $
################################################################################
# Package: VP1GuideLineSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1GuideLineSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/GeoPrimitives
   graphics/VP1/VP1Base
   graphics/VP1/VP1Utils
   PRIVATE
   GaudiKernel
   graphics/VP1/VP1HEPVis )

# External dependencies:
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1GuideLineSystems VP1GuideLineSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1GuideLineSystems
   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
   LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoPrimitives VP1Base VP1Utils
   Qt5::Core Qt5::Gui
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
   PRIVATE_LINK_LIBRARIES GaudiKernel VP1HEPVis )
