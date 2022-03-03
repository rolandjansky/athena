# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonNSWAsBuilt )

# External dependencies
find_package( nlohmann_json )

# Component(s) in the package:
atlas_add_library (MuonNSWAsBuilt
                   src/*.cxx
                   PUBLIC_HEADERS MuonNSWAsBuilt
                   LINK_LIBRARIES GeoPrimitives nlohmann_json::nlohmann_json)
