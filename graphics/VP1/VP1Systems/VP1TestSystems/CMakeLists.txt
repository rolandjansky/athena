################################################################################
# Package: VP1TestSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1TestSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          graphics/VP1/VP1Utils
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1TestSystems VP1TestSystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1TestSystems
                   INCLUDE_DIRS ${QT4_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES VP1Base VP1Utils ${QT4_LIBRARIES} GL StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel TrkSurfaces TrkParameters TrkTrack )

