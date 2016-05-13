################################################################################
# Package: QatPlotWidgets
################################################################################

# Declare the package name:
atlas_subdir( QatPlotWidgets )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatPlotting )

# External dependencies:
find_package( CLHEP )
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui QtSvg )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( QatPlotWidgets QatPlotWidgets/*.h src/*.h src/*.c* src/*.qrc  
                   PUBLIC_HEADERS QatPlotWidgets
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${QT4_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} QatPlotting ${QT4_LIBRARIES} GL )

