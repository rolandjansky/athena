# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODCnvInterfaces )

# Component(s) in the package:
atlas_add_library( xAODCnvInterfaces
                   PUBLIC_HEADERS xAODCnvInterfaces
                   LINK_LIBRARIES GaudiKernel )

