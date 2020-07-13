# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigIDUtils )

# Component(s) in the package:
atlas_add_component( TrigIDUtils
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel Identifier InDetIdentifier StoreGateLib )
