# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAlignToolInterfaces )

atlas_add_library( InDetAlignToolInterfaces
                   InDetAlignToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetAlignToolInterfaces
                   LINK_LIBRARIES GaudiKernel AthContainers )

