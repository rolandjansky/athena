# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecBackgroundEvent )

# Component(s) in the package:
atlas_add_library( RecBackgroundEvent
                   src/*.cxx
                   PUBLIC_HEADERS RecBackgroundEvent
                   LINK_LIBRARIES AthLinks AthenaKernel xAODCaloEvent xAODJet TrkSegment )

