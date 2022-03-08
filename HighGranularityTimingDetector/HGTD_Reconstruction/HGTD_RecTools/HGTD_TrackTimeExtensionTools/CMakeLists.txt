# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_TrackTimeExtensionTools )

# Component(s) in the package:
atlas_add_component( HGTD_TrackTimeExtensionTools
                    src/*.cxx
                    src/components/*.cxx
                    LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetPrepRawData
                    TrkGeometry TrkEventPrimitives TrkSurfaces TrkTrack
                    HGTD_RecToolInterfaces TrkExInterfaces HGTD_PrepRawData
                    TrkDetDescrUtils HGTD_ReadoutGeometry HGTD_Identifier
                    HGTD_RIO_OnTrack TrkToolInterfaces)

# Install files from the package:
atlas_install_headers( HGTD_TrackTimeExtensionTools )
