# $Id: CMakeLists.txt 784633 2016-11-16 16:16:34Z rbianchi $
################################################################################
# Package: VP1Base
################################################################################
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Set the name of the package:
atlas_subdir( VP1Base )

# Set up the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC DetectorDescription/GeoPrimitives
   PRIVATE graphics/VP1/VP1HEPVis )

# Find the external dependencies:
find_package( Qt5 COMPONENTS Core OpenGL Gui PrintSupport Widgets )
find_package( SoQt )
find_package( Coin3D )

# Generate UI files automatically:
# Note: add the "Widgets" component to "find_package( Qt5 ...)" if you have UI files, otherwise UIC, even if CMAKE_AUTOUIC is set to ON, is not run
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )

# Build the library. Remember that the auto-generated UI files are put under
# CMAKE_CURRENT_BINARY_DIR, so it needs to be added explicitly to the
# declaration of PRIVATE_INCLUDE_DIRS.
atlas_add_library( VP1Base VP1Base/*.h src/*.cxx src/*.qrc
   PUBLIC_HEADERS VP1Base
   INCLUDE_DIRS ${QT5_INCLUDE_DIRS} ${SOQT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${QT5_LIBRARIES} ${SOQT_LIBRARIES} ${COIN3D_LIBRARIES} GeoPrimitives
   PRIVATE_LINK_LIBRARIES VP1HEPVis )
