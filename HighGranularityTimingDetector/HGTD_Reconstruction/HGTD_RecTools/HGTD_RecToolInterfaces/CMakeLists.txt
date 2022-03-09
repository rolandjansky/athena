# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_RecToolInterfaces )

# Component(s) in the package:
atlas_add_library( HGTD_RecToolInterfaces
                   PUBLIC_HEADERS HGTD_RecToolInterfaces
                   LINK_LIBRARIES GaudiKernel HGTD_Identifier HGTD_PrepRawData
                   HGTD_RawData HGTD_ReadoutGeometry TrkTrack xAODTruth )
