#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoH62004Algs )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArGeoH62004Algs
                     src/*.cxx
                     src/components/*.cxx
		     PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests GeoModelUtilities LArReadoutGeometry CaloDetDescrLib AthContainers GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoH6Cryostats LArGeoTBECLib RDBAccessSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

