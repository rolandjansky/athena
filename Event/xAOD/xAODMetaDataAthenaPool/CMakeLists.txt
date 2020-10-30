# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODMetaDataAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODMetaDataAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODMetaData/FileMetaData.h xAODMetaData/FileMetaDataAuxInfo.h
                           TYPES_WITH_NAMESPACE xAOD::FileMetaData xAOD::FileMetaDataAuxInfo
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODMetaData )

