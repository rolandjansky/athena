# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AGDDModel )

# Component(s) in the package:
atlas_add_library( AGDDModel
                   src/*.cxx
                   PUBLIC_HEADERS AGDDModel
                   LINK_LIBRARIES AGDDKernel )
