# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( HGTD_EventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES HGTD_RawData/HGTD_RDOContainer.h
         HGTD_PrepRawData/HGTD_ClusterContainer.h
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib HGTD_RawData HGTD_PrepRawData
   HGTD_EventTPCnv )
