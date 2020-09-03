# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( IOVDbAthenaPoolPoolCnv
                           src/*.cxx
                           FILES IOVDbDataModel/IOVMetaDataContainer.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib IOVDbDataModel IOVDbTPCnv )
