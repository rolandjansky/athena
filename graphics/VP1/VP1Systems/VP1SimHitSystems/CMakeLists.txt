################################################################################
# Package: VP1SimHitSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1SimHitSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoAdaptors
                          DetectorDescription/GeoPrimitives
                          ForwardDetectors/ForwardSimulation/ForwardRegion_SimEv
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonSimEvent
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )
find_package( Qt5 COMPONENTS Core Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Component(s) in the package:
atlas_add_library( VP1SimHitSystems VP1SimHitSystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1SimHitSystems
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES VP1Base Qt5::Core Qt5::Widgets GL StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${EIGEN_LIBRARIES} GeoAdaptors GeoPrimitives ForwardRegion_SimEv InDetSimEvent MuonSimEvent VP1Utils )

