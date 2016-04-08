################################################################################
# Package: GeometryJiveXML
################################################################################

# Declare the package name:
atlas_subdir( GeometryJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          graphics/JiveXML
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          TileCalorimeter/TileDetDescr )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( GeometryJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps JiveXMLLib CaloDetDescrLib StoreGateLib SGtests GeoModelKernel GaudiKernel InDetIdentifier InDetReadoutGeometry LArReadoutGeometry MuonReadoutGeometry MuonIdHelpersLib TileDetDescr )

# Install files from the package:
atlas_install_headers( GeometryJiveXML )

