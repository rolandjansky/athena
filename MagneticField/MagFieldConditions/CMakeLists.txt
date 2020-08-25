# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MagFieldConditions )

# Component(s) in the package:
atlas_add_library( MagFieldConditions
                   src/*.cxx
                   PUBLIC_HEADERS MagFieldConditions
                   LINK_LIBRARIES AthenaKernel GaudiKernel MagFieldElements )
