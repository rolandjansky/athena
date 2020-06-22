################################################################################
# Package: LArG4FastSimulation
################################################################################

# Declare the package name:
atlas_subdir( LArG4FastSimulation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoSpecialShapes
			  DetectorDescription/GeoPrimitives
                          LArCalorimeter/LArG4/LArG4Code
                          LArCalorimeter/LArG4/LArG4ShowerLibSvc
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          LArCalorimeter/LArSimEvent
                          Simulation/G4Atlas/G4AtlasTools 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( XercesC )
find_package( GeoModelCore )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( LArG4FastSimulation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEOMODELCORE_LIBRARIES} GaudiKernel StoreGateLib SGtests GeoSpecialShapes LArG4Code LArReadoutGeometry LArSimEvent G4AtlasToolsLib GeoPrimitives LArG4ShowerLibSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

