# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBDetDescrAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TBDetDescrAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TBDetDescr/TBElementContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib TBDetDescr )

