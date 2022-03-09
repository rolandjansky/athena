# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_RecToolInterfaces )

# Component(s) in the package:
atlas_add_library( HGTD_RecToolInterfaces
                   PUBLIC_HEADERS HGTD_RecToolInterfaces
                   LINK_LIBRARIES Identifier GaudiKernel HGTD_ReadoutGeometry
                   HGTD_Identifier HGTD_RawData HGTD_PrepRawData)
