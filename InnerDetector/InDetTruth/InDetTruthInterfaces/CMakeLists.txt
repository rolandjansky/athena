# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTruthInterfaces )

atlas_add_library( InDetTruthInterfaces
                   InDetTruthInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetTruthInterfaces
                   LINK_LIBRARIES GaudiKernel )
