# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IdDictDetDescr )

# Component(s) in the package:
atlas_add_library( IdDictDetDescr
                   src/IdDictManager.cxx
                   PUBLIC_HEADERS IdDictDetDescr
                   LINK_LIBRARIES IdDict
                   PRIVATE_LINK_LIBRARIES AthenaKernel Identifier )
