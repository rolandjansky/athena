# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkAssociationTools )

# Component(s) in the package:
atlas_add_component( TrkAssociationTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkToolInterfaces GaudiKernel TrkDetElementBase TrkEventUtils TrkRIO_OnTrack TrkCompetingRIOsOnTrack MuonIdHelpersLib TrkTrack TrkEventUtils StoreGateLib )
