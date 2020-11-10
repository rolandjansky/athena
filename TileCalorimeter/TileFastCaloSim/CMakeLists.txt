# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileFastCaloSim )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TileFastCaloSim
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel EventContainers GaudiKernel GeoModelInterfaces ISF_FastCaloSimEvent TileConditionsLib TileDetDescr TileGeoModelLib TileG4InterfacesLib TileSimEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )

