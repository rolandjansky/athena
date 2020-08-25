# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BeamPipeGeoModel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( BeamPipeGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS BeamPipeGeoModel
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeoModelInterfaces RDBAccessSvcLib SGTools StoreGateLib )

atlas_add_component( BeamPipeGeoModel
                     src/components/*.cxx
                     LINK_LIBRARIES BeamPipeGeoModelLib )

# Test(s) in the package:
atlas_add_test( BeamPipeGMConfig_test
                SCRIPT test/BeamPipeGMConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
