# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODEventFormatAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEventFormatAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODEventFormat/EventFormat.h
                           TYPES_WITH_NAMESPACE xAOD::EventFormat
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEventFormat )

