################################################################################
# Package: VP12DGeometrySystems
################################################################################

# Declare the package name:
atlas_subdir( VP12DGeometrySystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          graphics/VP1/VP1Base
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          TileCalorimeter/TileDetDescr )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )
find_package( Qt5 )

# Run the QT build steps automatically.
set( CMAKE_AUTOMOC TRUE )
set( CMAKE_AUTOUIC TRUE )

# Component(s) in the package:
atlas_add_library( VP12DGeometrySystems
		   VP12DGeometrySystems/*.h src/*.cxx src/*.ui
                   PUBLIC_HEADERS VP12DGeometrySystems
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives VP1Base StoreGateLib SGtests CaloDetDescrLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} IdDictDetDescr InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry LArReadoutGeometry MuonReadoutGeometry TileDetDescr )

