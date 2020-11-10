# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventShapeAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( EventShapeAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventShapeEvent/EventShapeStore.h
                           TYPES_WITH_NAMESPACE 
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib EventShapeEvent EventShapeTPCnv )
