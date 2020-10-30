# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArTBEvent )

# Component(s) in the package:
atlas_add_library( LArTBEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArTBEvent
                   LINK_LIBRARIES AthContainers AthenaKernel )

