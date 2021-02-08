# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: PhantomCalorimeter
################################################################################

# Declare the package name:
atlas_subdir( PhantomCalorimeter )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( PhantomCalorimeter
                     src/*.cc
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} CaloIdentifier StoreGateLib SGtests GaudiKernel G4AtlasToolsLib GeoMaterial2G4 GeoModelInterfaces TileSimEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/phantombarrel.dtd share/phantombarrel.xml share/TileAncillary.mac share/phantomvis.mac )

