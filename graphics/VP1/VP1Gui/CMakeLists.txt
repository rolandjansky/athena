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
   PRIVATE graphics/VP1/VP1UtilsBase )

# External dependencies:
find_package( Coin3D )
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui QtWebKit QtNetwork )
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
   LINK_LIBRARIES VP1Base Qt4::QtCore Qt4::QtOpenGL Qt4::QtGui Qt4::QtWebKit
   Qt4::QtNetwork
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${SOQT_LIBRARIES} VP1UtilsBase )

   
# Install files from the package:
##atlas_install_scripts( share/* ) # installs into bin/
atlas_install_runtime( share/* ) # install into share/
   