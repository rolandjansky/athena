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
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          TileCalorimeter/TileDetDescr )

# External dependencies:
find_package( GeoModelCore )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( GeometryJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps JiveXMLLib CaloDetDescrLib StoreGateLib SGtests GaudiKernel InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry LArReadoutGeometry MuonReadoutGeometry MuonIdHelpersLib TileDetDescr )

# Install files from the package:
atlas_install_headers( GeometryJiveXML )

