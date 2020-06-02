################################################################################
# Package: Geo2G4
################################################################################

# Declare the package name:
atlas_subdir( Geo2G4 )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoSpecialShapes
                          DetectorDescription/GeoModel/GeoModelUtilities
			  DetectorDescription/GeoPrimitives
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/SimHelpers
                          Simulation/G4Utilities/GeoMaterial2G4 )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS MathCore RIO Core Tree Hist pthread )
find_package( XercesC )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( Geo2G4Lib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps GeoSpecialShapes G4AtlasToolsLib SimHelpers GeoMaterial2G4 AthenaKernel GeoModelInterfaces GeoModelUtilities StoreGateLib SGtests GeoPrimitives )

atlas_add_component( Geo2G4
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests GeoSpecialShapes G4AtlasToolsLib SimHelpers GeoMaterial2G4 Geo2G4Lib )

atlas_add_dictionary( LArWheelSolidCheckerDict
                      src/LArWheelSolidDDProxy.h
                      src/lcg_dict/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests GeoSpecialShapes SimHelpers GeoMaterial2G4 Geo2G4Lib )

