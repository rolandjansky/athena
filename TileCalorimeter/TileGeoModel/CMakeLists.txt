# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileGeoModel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( TileGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS TileGeoModel
                   PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoModelUtilities TileDetDescr StoreGateLib SGtests CaloDetDescrLib TileConditionsLib
                   PRIVATE_LINK_LIBRARIES CaloIdentifier SGTools GaudiKernel TileIdentifier )

atlas_add_component( TileGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES TileGeoModelLib )


# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )


if( NOT SIMULATIONBASE )
  atlas_add_test( TileGMConfig    SCRIPT python -m TileGeoModel.TileGMConfig POST_EXEC_SCRIPT nopost.sh )
endif()

