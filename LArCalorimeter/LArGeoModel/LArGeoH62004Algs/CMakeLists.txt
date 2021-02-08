#Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoH62004Algs )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( LArGeoH62004Algs
                     src/*.cxx
                     src/components/*.cxx
		     PRIVATE_INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                     PRIVATE_LINK_LIBRARIES ${GEOMODEL_LIBRARIES} StoreGateLib SGtests GeoModelUtilities LArReadoutGeometry CaloDetDescrLib AthContainers GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoH6Cryostats LArGeoTBECLib RDBAccessSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

