# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MagFieldInterfaces )

# Component(s) in the package:
atlas_add_library( MagFieldInterfaces
                   PUBLIC_HEADERS MagFieldInterfaces
                   LINK_LIBRARIES GeoPrimitives GaudiKernel )
