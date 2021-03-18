# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCablingTools )

# Component(s) in the package:
atlas_add_library( MuonCablingTools
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingTools
                   LINK_LIBRARIES GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaKernel )
