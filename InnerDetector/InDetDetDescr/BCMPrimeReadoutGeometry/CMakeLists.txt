# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BCMPrimeReadoutGeometry )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

atlas_add_library( BCMPrimeReadoutGeometry
  src/*.cxx
  PUBLIC_HEADERS BCMPrimeReadoutGeometry
  LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel CxxUtils GeoPrimitives
  PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities )

