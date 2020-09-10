# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StlAthenaPoolCnv )

# Component(s) in the package:
atlas_add_component( StlAthenaPoolCnv
                     src/*.cxx
                     LINK_LIBRARIES SGTools AthenaPoolCnvSvcLib )
