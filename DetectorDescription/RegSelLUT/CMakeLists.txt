# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RegSelLUT )

# Component(s) in the package:
atlas_add_library( RegSelLUT
                   src/*.cxx
                   PUBLIC_HEADERS RegSelLUT
                   LINK_LIBRARIES AthenaKernel Identifier GaudiKernel IRegionSelector )
