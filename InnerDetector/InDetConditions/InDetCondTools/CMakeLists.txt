# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCondTools )

# Component(s) in the package:
atlas_add_library( InDetCondTools
                   PUBLIC_HEADERS InDetCondTools
                   LINK_LIBRARIES GeoPrimitives GaudiKernel )
