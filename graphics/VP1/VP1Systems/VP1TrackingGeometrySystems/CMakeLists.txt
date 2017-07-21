################################################################################
# Package: VP1TrackingGeometrySystems
################################################################################

# Declare the package name:
atlas_subdir( VP1TrackingGeometrySystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelKernel
                          Tracking/TrkDetDescr/TrkVolumes
                          graphics/VP1/VP1Base
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          graphics/VP1/VP1HEPVis
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )
find_package( Qt5 COMPONENTS Core Gui  )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)




# Component(s) in the package:
atlas_add_library( VP1TrackingGeometrySystems VP1TrackingGeometrySystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1TrackingGeometrySystems
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoModelKernel TrkVolumes VP1Base GL StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry VP1HEPVis VP1Utils )

