# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileSimEvent )

# Component(s) in the package:
atlas_add_library( TileSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS TileSimEvent
                   LINK_LIBRARIES AthenaKernel Identifier HitManagement
                   PRIVATE_LINK_LIBRARIES CaloIdentifier TileConditionsLib )

atlas_add_dictionary( TileSimEventDict
                      TileSimEvent/TileSimEventDict.h
                      TileSimEvent/selection.xml
                      LINK_LIBRARIES TileSimEvent )
