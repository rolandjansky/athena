# $Id: CMakeLists.txt 724584 2016-02-17 15:40:28Z krasznaa $

# Set the name of the package:
atlas_subdir( VP1Base )

# Set up the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC DetectorDescription/GeoPrimitives
   PRIVATE graphics/VP1/VP1HEPVis )

# Find the external dependencies:
find_package( Qt4 COMPONENTS QtCore QtGui QtOpenGL )
find_package( SoQt )
find_package( Coin3D )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )

# Build the library. Remember that the auto-generated UI files are put under
# CMAKE_CURRENT_BINARY_DIR, so it needs to be added explicitly to the
# declaration.
atlas_add_library( VP1Base VP1Base/*.h src/*.cxx src/*.qrc
   PUBLIC_HEADERS VP1Base
   INCLUDE_DIRS ${SOQT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES Qt4::QtCore Qt4::QtGui ${SOQT_LIBRARIES} ${COIN3D_LIBRARIES}
   GeoPrimitives
   PRIVATE_LINK_LIBRARIES Qt4::QtOpenGL VP1HEPVis )
