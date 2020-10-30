# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_Identifier )

# Component(s) in the package:
atlas_add_library( LUCID_Identifier
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_Identifier
                   LINK_LIBRARIES AthenaKernel AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

