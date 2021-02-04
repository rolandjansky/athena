# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoModelSvc )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( GeoModelSvc
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel SGTools
                  StoreGateLib GeoModelInterfaces GeoModelUtilities GaudiKernel
                  RDBAccessSvcLib EventInfoMgtLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
