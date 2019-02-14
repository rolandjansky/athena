################################################################################
# Package: VP1Gui
################################################################################
# Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Declare the package name:
atlas_subdir( VP1Gui )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC graphics/VP1/VP1Base
   PUBLIC graphics/VP1/VP1UtilsBase 
   )

# External dependencies:
find_package( Qt5 COMPONENTS Core OpenGL Gui Network PrintSupport Widgets )
find_package( Coin3D )
find_package( SoQt )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )

# Component(s) in the package:
atlas_add_library( VP1Gui VP1Gui/*.h src/*.h src/*.cxx src/*.qrc
   PUBLIC_HEADERS VP1Gui
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${SOQT_INCLUDE_DIRS}
   ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES VP1Base Qt5::Core Qt5::OpenGL Qt5::Gui VP1UtilsBase
   Qt5::PrintSupport
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${SOQT_LIBRARIES} Qt5::Network )

