# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( LArDetDescr )

# External dependencies.
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( LArDetDescr
   LArDetDescr/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} GaudiKernel AthenaBaseComps
   CaloDetDescrLib CaloGeoHelpers RDBAccessSvcLib StoreGateLib CaloIdentifier
   GeoModelInterfaces GeoModelUtilities GeoPrimitives )

# Install files from the package.
atlas_install_joboptions( share/*.py )
