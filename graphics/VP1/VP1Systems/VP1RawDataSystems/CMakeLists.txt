################################################################################
# Package: VP1RawDataSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1RawDataSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          graphics/VP1/VP1Base
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelKernel
                          Event/EventPrimitives
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          InnerDetector/InDetRawEvent/InDetRawData
                          graphics/VP1/VP1HEPVis
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# Component(s) in the package:
atlas_add_library( VP1RawDataSystems VP1RawDataSystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1RawDataSystems
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   FILES LUCID_RawEvent/LUCID_Digit.h
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoModelKernel EventPrimitives LUCID_RawEvent GaudiKernel InDetIdentifier InDetReadoutGeometry InDetBCM_RawData InDetRawData VP1HEPVis VP1Utils )

