# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( ALFA_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ALFA_SimEv/ALFA_HitCollection.h ALFA_SimEv/ALFA_ODHitCollection.h ALFA_RawEv/ALFA_DigitCollection.h ALFA_RawEv/ALFA_ODDigitCollection.h ALFA_RawEv/ALFA_RawDataContainer.h ALFA_LocRecEv/ALFA_LocRecEvCollection.h ALFA_LocRecEv/ALFA_LocRecODEvCollection.h ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h ALFA_GloRecEv/ALFA_GloRecEvCollection.h ALFA_CLinkEv/ALFA_CLinkEvent.h
                           LINK_LIBRARIES StoreGateLib AthenaPoolCnvSvcLib ALFA_CLinkEv ALFA_EventTPCnv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv GaudiKernel )
