# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AraTool )

# Component(s) in the package:
atlas_add_library( AraTool
                   src/*.cxx
                   PUBLIC_HEADERS AraTool
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel )
