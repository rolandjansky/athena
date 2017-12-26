################################################################################
# Package: VP1UtilitySystems
################################################################################

# Declare the package name:
atlas_subdir( VP1UtilitySystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          graphics/Qat/QatDataAnalysis
                          graphics/Qat/QatPlotWidgets
                          graphics/Qat/QatPlotting )

# External dependencies:
find_package( Coin3D )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Qt5 COMPONENTS Core Gui  Widgets PrintSupport)

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)



# Component(s) in the package:
atlas_add_library( VP1UtilitySystems VP1UtilitySystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1UtilitySystems
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES VP1Base GL Qt5::PrintSupport
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${COIN3D_LIBRARIES} QatDataAnalysis QatPlotWidgets QatPlotting )

