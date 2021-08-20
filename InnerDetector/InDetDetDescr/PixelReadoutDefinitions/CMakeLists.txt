# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelReadoutDefinitions )

# Component(s) in the package:
atlas_add_library( PixelReadoutDefinitionsLib
                   PixelReadoutDefinitions/*.h
                   INTERFACE
                   PUBLIC_HEADERS PixelReadoutDefinitions )
