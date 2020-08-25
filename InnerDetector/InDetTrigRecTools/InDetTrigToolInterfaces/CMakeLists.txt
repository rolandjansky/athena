# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigToolInterfaces )

# Component(s) in the package:
atlas_add_library( InDetTrigToolInterfacesLib
                   InDetTrigToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetTrigToolInterfaces
                   LINK_LIBRARIES GaudiKernel Identifier InDetPrepRawData )
