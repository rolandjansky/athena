# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( AFP_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES AFP_SimEv/AFP_TDSimHitCollection.h AFP_SimEv/AFP_SIDSimHitCollection.h AFP_DigiEv/AFP_TDDigiCollection.h AFP_DigiEv/AFP_SiDigiCollection.h AFP_LocRecoEv/AFP_TDLocRecoEvCollection.h AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AFP_DigiEv AFP_EventTPCnv AFP_LocRecoEv AFP_SimEv GaudiKernel )
