# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GenzModuleEvent )

# The dependencies of the package:

# Component(s) in the package:
atlas_add_library( GenzModuleEvent
                   src/*.cxx
                   PUBLIC_HEADERS GenzModuleEvent
                   LINK_LIBRARIES AtlasHepMCLib )

