# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BCM_CollisionTime )

atlas_add_library( BCM_CollisionTimeLib
                   BCM_CollisionTime/*.h
                   INTERFACE
                   PUBLIC_HEADERS BCM_CollisionTime
                   LINK_LIBRARIES AthenaKernel )

