# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcConditions )

# Component(s) in the package:
atlas_add_library( ZdcConditions
                   src/*.cxx
                   PUBLIC_HEADERS ZdcConditions
                   LINK_LIBRARIES Identifier ZdcIdentifier
                   PRIVATE_LINK_LIBRARIES StoreGateLib )
