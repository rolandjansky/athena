# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( LUCID_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LUCID_SimEvent/LUCID_SimHitCollection.h LUCID_RawEvent/LUCID_DigitContainer.h LUCID_RawEvent/LUCID_RawDataContainer.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib LUCID_EventTPCnv LUCID_RawEvent GaudiKernel )
