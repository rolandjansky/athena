# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SGAudCore )

# Component(s) in the package:
atlas_add_library( SGAudCore
                   src/*.cxx
                   PUBLIC_HEADERS SGAudCore
                   LINK_LIBRARIES GaudiKernel )

