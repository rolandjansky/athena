# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetEDM )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_library( JetEDM
                   Root/*.cxx
                   PUBLIC_HEADERS JetEDM
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} AsgTools xAODBase xAODJet xAODTracking
                   PRIVATE_LINK_LIBRARIES xAODMuon )
