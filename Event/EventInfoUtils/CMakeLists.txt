# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventInfoUtils )

# Libraries(s) in the package:
atlas_add_library( EventInfoUtils
                   src/*.cxx
                   PUBLIC_HEADERS EventInfoUtils
                   LINK_LIBRARIES EventInfo xAODEventInfo StoreGateLib
                   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel SGTools )
