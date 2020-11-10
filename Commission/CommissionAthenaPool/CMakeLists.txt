# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CommissionAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( CommissionAthenaPoolPoolCnv
                           src/*.cxx
                           FILES CommissionEvent/ComTime.h
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel CommissionEvent AthenaPoolCnvSvcLib )

