# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventTagAthenaPOOL )

# Component(s) in the package:
atlas_add_poolcnv_library( EventTagAthenaPOOLPoolCnv
                           src/*.cxx
                           FILES TagEvent/RawInfoSummaryForTag.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib EventTagTPCnv TagEvent )
