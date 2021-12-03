# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_IdCnv )

# Component(s) in the package:
atlas_add_component( HGTD_IdCnv
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps StoreGateLib DetDescrCnvSvcLib HGTD_Identifier
   IdDictDetDescr )
