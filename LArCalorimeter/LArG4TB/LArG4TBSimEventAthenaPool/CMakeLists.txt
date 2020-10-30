# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArG4TBSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( LArG4TBSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArG4TBSimEvent/LArG4H6FrontHitCollection.h LArG4TBSimEvent/LArG4H6LeakHitCollection.h LArG4TBSimEvent/LArG4H6WarmTCHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArG4TBSimEvent )

