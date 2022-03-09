# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_PadClusterizationTools )

# Component(s) in the package:
atlas_add_component( HGTD_PadClusterizationTools
                    src/*.cxx
                    src/components/*.cxx
                    LINK_LIBRARIES AthenaBaseComps GaudiKernel
                    HGTD_RecToolInterfaces HGTD_PrepRawData HGTD_ReadoutGeometry)

# Install files from the package:
atlas_install_headers( HGTD_PadClusterizationTools )
