# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrigMuonToolInterfaces
                   TrigMuonToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigMuonToolInterfaces
                   LINK_LIBRARIES GaudiKernel TrigInDetEvent TrigMuonEvent xAODMuon xAODTrigMuon )
