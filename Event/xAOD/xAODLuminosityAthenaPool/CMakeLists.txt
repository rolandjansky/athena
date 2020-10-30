# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODLuminosityAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODLuminosityAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODLuminosity/LumiBlockRangeContainer.h xAODLuminosity/LumiBlockRangeAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::LumiBlockRangeContainer xAOD::LumiBlockRangeAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODLuminosity )

