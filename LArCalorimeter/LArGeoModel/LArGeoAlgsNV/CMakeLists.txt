#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoAlgsNV )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArGeoAlgsNV
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoModelUtilities LArReadoutGeometry SGTools StoreGateLib SGtests DetDescrConditions GaudiKernel LArGeoBarrel LArGeoCode LArGeoEndcap LArGeoRAL LArGeoTBBarrel LArHV RDBAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

if( NOT SIMULATIONBASE )
  atlas_add_test( LArGMConfigTest SCRIPT python -m LArGeoAlgsNV.LArGMConfig POST_EXEC_SCRIPT nopost.sh )
endif()
