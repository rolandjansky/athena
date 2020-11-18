# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( RecEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES RecEvent/RecoTimingObj.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GaudiKernel RecEvent RecEventTPCnv )
