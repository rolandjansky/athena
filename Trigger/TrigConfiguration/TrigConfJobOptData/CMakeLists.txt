# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfJobOptData )

# Component(s) in the package:
atlas_add_library( TrigConfJobOptData
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfJobOptData
                   LINK_LIBRARIES TrigConfL1Data )

