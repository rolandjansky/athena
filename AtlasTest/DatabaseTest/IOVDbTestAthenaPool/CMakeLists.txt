# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbTestAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( IOVDbTestAthenaPoolPoolCnv
                           src/*.cxx
                           FILES IOVDbTestConditions/IOVDbTestAmdbCorrection.h IOVDbTestConditions/IOVDbTestMDTEleMap.h IOVDbTestConditions/IOVDbTestMDTEleMapColl.h
                           MULT_CHAN_TYPES IOVDbTestMDTEleMapColl
                           LINK_LIBRARIES AthenaPoolUtilities IOVDbTestConditions AthenaPoolCnvSvcLib )
