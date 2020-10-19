# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthTrackInterfaces )

atlas_add_library( TrkTruthTrackInterfaces
                   TrkTruthTrackInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkTruthTrackInterfaces
                   LINK_LIBRARIES GaudiKernel AtlasHepMCLib TrkSegment )

