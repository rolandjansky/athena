# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( CaloSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES CaloSimEvent/CaloCalibrationHitContainer.h
                           LINK_LIBRARIES CaloSimEventTPCnv CaloSimEvent AthenaPoolCnvSvcLib GaudiKernel )
