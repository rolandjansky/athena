# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_GeoModel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( ALFA_GeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel AthenaPoolUtilities GeoModelInterfaces GeoModelUtilities ALFA_Geometry StoreGateLib GaudiKernel RDBAccessSvcLib GeoPrimitives )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

