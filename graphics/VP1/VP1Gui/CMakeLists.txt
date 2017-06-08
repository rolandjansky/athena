################################################################################
# Package: VP1Gui
################################################################################
# Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# Declare the package name:
atlas_subdir( VP1Gui )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC graphics/VP1/VP1Base
   PRIVATE graphics/VP1/VP1UtilsBase )

# External dependencies:

# TEST 1
#find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui QtWebKit QtNetwork )
#find_package( Qt5 COMPONENTS Core OpenGL Gui WebEngine Network REQUIRED )

# TEST 2
# default version shipped with LCG release (currently: 5.6.0 in LCG_88)
#find_package( Qt5 5.6.2 COMPONENTS Core OpenGL Gui Network REQUIRED HINTS ${QT5_ROOT} )

# TEST 3
# requiring the Qt5 5.6.2 shipped in LCG nightly build
#set( QT562_ROOT /cvmfs/sft.cern.ch/lcg/nightlies/dev4/Wed/qt5/5.6.2 )
#set( ENV(CMAKE_PREFIX_PATH) ${QT562_ROOT} )
#find_package( Qt5 5.6.2 COMPONENTS Core OpenGL Gui Network REQUIRED HINTS ${QT5_ROOT} )

# TEST $


find_package( Coin3D REQUIRED )
find_package( SoQt REQUIRED )

# set C++11 explicitely
set (CMAKE_CXX_STANDARD 11)

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
   LINK_LIBRARIES VP1Base Qt5::Core Qt5::OpenGL Qt5::Gui 
   Qt5::Network
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${SOQT_LIBRARIES} VP1UtilsBase )

   
# Install files from the package:
##atlas_install_scripts( share/* ) # installs into bin/
atlas_install_runtime( share/* ) # install into share/
   