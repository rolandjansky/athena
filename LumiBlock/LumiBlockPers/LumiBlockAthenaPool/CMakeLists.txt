# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LumiBlockAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( LumiBlockAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LumiBlockData/LumiBlockCollection.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib LumiBlockData LumiBlockTPCnv )
