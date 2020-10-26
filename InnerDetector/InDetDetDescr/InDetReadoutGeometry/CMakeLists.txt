# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( InDetReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS InDetReadoutGeometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel AtlasDetDescr CxxUtils GeoPrimitives Identifier GaudiKernel InDetIdentifier TrkDetElementBase TrkSurfaces TrkEventPrimitives StoreGateLib DetDescrConditions InDetCondTools
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities IdDictDetDescr GeoModelUtilities )
