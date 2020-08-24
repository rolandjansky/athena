################################################################################
# Package: LArG4H6SD
################################################################################

# Declare the package name:
atlas_subdir( LArG4H6SD )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4H6SD
                     src/*.cxx
                     src/*.cc
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEANT4_LIBRARIES} GaudiKernel CaloG4SimLib CaloSimEvent AthenaBaseComps AthenaKernel StoreGateLib SGtests GeoModelUtilities Identifier GeneratorObjects LArG4Code LArG4RunControl LArG4TBSimEvent LArReadoutGeometry LArSimEvent G4AtlasInterfaces G4AtlasToolsLib SimHelpers HitManagement MCTruth TBEvent PathResolver GeoModelInterfaces RDBAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
