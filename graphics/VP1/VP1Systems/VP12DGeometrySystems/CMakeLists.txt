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
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          TileCalorimeter/TileDetDescr )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( VP12DGeometrySystems
                   src/*.c*
                   PUBLIC_HEADERS VP12DGeometrySystems
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives VP1Base StoreGateLib SGtests CaloDetDescrLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES GeoModelKernel IdDictDetDescr InDetIdentifier InDetReadoutGeometry LArReadoutGeometry MuonReadoutGeometry TileDetDescr )

