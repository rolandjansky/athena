# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCalibAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigT1CaloCalibAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrigT1CaloCalibConditions/L1CaloPedestalContainer.h TrigT1CaloCalibConditions/L1CaloRampDataContainer.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloCalibConditions )

