################################################################################
# Package: QatPresenter
################################################################################

# Declare the package name:
atlas_subdir( QatPresenter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatDataAnalysis
                          graphics/Qat/QatPlotWidgets
                          graphics/Qat/QatSystemInterfaces )

# External dependencies:
find_package( Qt4 )

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
atlas_add_library( QatPresenter QatPresenter/*.h src/*.h src/*.c* src/*.qrc  
                   PUBLIC_HEADERS QatPresenter
                   INCLUDE_DIRS ${QT4_INCLUDE_DIRS}
                   LINK_LIBRARIES QatDataAnalysis QatPlotWidgets QatSystemInterfaces ${QT4_LIBRARIES} )

