# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PFOHistUtils )

# Component(s) in the package:
atlas_add_library( PFOHistUtils
                   src/*.cxx
                   PUBLIC_HEADERS PFOHistUtils
                   LINK_LIBRARIES xAODCaloEvent xAODPFlow xAODTracking TrkValHistUtils )
