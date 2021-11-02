# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( HGTD_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES HGTD_PrepRawData/HGTD_ClusterContainer.h
                           TYPES_WITH_NAMESPACE HGTD::HGTD_ClusterContainer
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel HGTD_PrepRawData HGTD_EventTPCnv )
