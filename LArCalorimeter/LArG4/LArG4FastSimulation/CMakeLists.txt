# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArG4FastSimulation )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArG4FastSimulation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEOMODELCORE_LIBRARIES} GaudiKernel StoreGateLib GeoSpecialShapes LArG4Code LArReadoutGeometry LArSimEvent G4AtlasToolsLib GeoPrimitives LArG4ShowerLibSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

