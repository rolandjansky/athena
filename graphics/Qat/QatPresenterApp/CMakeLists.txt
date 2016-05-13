################################################################################
# Package: QatPresenterApp
################################################################################

# Declare the package name:
atlas_subdir( QatPresenterApp )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatPresenter )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set additional compilation flags
SET(MY_GCC_COMPILE_FLAGS "-g -O0")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MY_GCC_COMPILE_FLAGS}" )

include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_executable( present
                      src/present.cpp
                      INCLUDE_DIRS ${QT4_INCLUDE_DIRS}
                      LINK_LIBRARIES ${QT4_LIBRARIES} GL QatPresenter)

