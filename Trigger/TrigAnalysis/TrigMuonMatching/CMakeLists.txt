# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonMatching )

# Component(s) in the package:
atlas_add_library( TrigMuonMatchingLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS TrigMuonMatching
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps GaudiKernel xAODRootAccess xAODTrigMuon xAODTrigger
                   LINK_LIBRARIES AsgTools TrigDecisionToolLib xAODMuon )

atlas_add_component( TrigMuonMatching
                     src/components/*.cxx
                     LINK_LIBRARIES TrigMuonMatchingLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

