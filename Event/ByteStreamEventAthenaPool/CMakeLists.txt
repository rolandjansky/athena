# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( ByteStreamEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ByteStreamData/ByteStreamMetadataContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib ByteStreamData ByteStreamData_test ByteStreamEventTPCnv )

