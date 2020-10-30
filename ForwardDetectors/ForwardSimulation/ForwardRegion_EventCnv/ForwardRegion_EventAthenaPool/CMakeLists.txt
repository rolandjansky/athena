# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardRegion_EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( ForwardRegion_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ForwardRegion_SimEv/SimulationHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP ForwardRegion_EventTPCnv ForwardRegion_SimEv GaudiKernel )
