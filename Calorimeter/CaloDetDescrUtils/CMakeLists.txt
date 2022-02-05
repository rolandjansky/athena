# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( CaloDetDescrUtils )

# Component(s) in the package.
atlas_add_library( CaloDetDescrUtils
		   src/*.cxx
		   PUBLIC_HEADERS CaloDetDescrUtils
		   PRIVATE_LINK_LIBRARIES GaudiKernel CaloIdentifier CaloConditions GeoModelUtilities
		                          LArReadoutGeometry TileDetDescr StoreGateLib RDBAccessSvcLib )
