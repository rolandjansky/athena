# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RunVP1Light )

# External dependencies:
find_package( Qt5 COMPONENTS Core OpenGL )
find_package( Coin3D )
find_package( SoQt )

#message( "QT5_ROOT: ${QT5_ROOT} - QTDIR: $ENV{QTDIR} - Qt5_FOUND: ${Qt5_FOUND} - QTLIB: $ENV{QTLIB} - QTINC: $ENV{QTINC}" )

if( APPLE )
   set(CMAKE_MACOSX_RPATH 1)
endif()

# Component(s) in the package:
atlas_add_executable( vp1light RunVP1Light/*.h src/*.cxx
   INCLUDE_DIRS ${SOQT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
   LINK_LIBRARIES VP1Gui Qt5::Core VP1UtilsBase ${SOQT_LIBRARIES} ${COIN3D_LIBRARIES} )
   
# RPATH for AppImage
# if(UNIX AND NOT APPLE)
set_target_properties(vp1light PROPERTIES 
                     INSTALL_RPATH "$ORIGIN/../lib:$$ORIGIN/"
                     INSTALL_RPATH_USE_LINK_PATH TRUE)
# endif()

# Install files from the package:
##atlas_install_scripts( share/* ) # installs into bin/
atlas_install_runtime( share/* ) # install into share/
