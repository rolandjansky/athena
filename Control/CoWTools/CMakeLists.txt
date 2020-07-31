# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CoWTools )

# Component(s) in the package:
atlas_add_library( CoWTools
   CoWTools/*.h src/*.cxx
   PUBLIC_HEADERS CoWTools
   LINK_LIBRARIES GaudiKernel )
