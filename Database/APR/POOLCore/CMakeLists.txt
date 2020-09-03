# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( POOLCore )

# Component(s) in the package:
atlas_add_library( POOLCore
                   src/*.cpp
                   PUBLIC_HEADERS POOLCore
                   LINK_LIBRARIES GaudiKernel )
