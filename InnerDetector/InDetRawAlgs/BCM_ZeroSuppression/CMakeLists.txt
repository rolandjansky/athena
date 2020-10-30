# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BCM_ZeroSuppression )

# Component(s) in the package:
atlas_add_component( BCM_ZeroSuppression
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetBCM_RawData StoreGateLib )
