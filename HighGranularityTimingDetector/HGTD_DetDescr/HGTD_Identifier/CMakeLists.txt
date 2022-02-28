# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_Identifier )

# Component(s) in the package:
atlas_add_library( HGTD_Identifier
   HGTD_Identifier/*.h src/HGTD_ID.cxx
   PUBLIC_HEADERS HGTD_Identifier
   LINK_LIBRARIES AthenaKernel AtlasDetDescr IdDict Identifier
   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( HGTD_IdentifierDict
   HGTD_Identifier/HGTD_IdentifierDict.h HGTD_Identifier/selection.xml
   LINK_LIBRARIES HGTD_Identifier )
