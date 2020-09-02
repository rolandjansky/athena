# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( LArDetDescr )

# External dependencies.
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArDetDescr
   LArDetDescr/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GaudiKernel AthenaBaseComps
   CaloDetDescrLib CaloGeoHelpers RDBAccessSvcLib StoreGateLib CaloIdentifier
   GeoModelInterfaces GeoModelUtilities GeoPrimitives )

# Install files from the package.
atlas_install_joboptions( share/*.py )
