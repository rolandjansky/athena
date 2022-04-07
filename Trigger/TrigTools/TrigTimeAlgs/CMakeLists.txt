# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTimeAlgs )

# Component(s) in the package:
atlas_add_library( TrigTimeAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTimeAlgs
                   LINK_LIBRARIES AthenaKernel )
