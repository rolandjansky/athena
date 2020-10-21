# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrackLink )

atlas_add_library( TrkTrackLink
                   TrkTrackLink/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkTrackLink
                   LINK_LIBRARIES TrkNeutralParameters TrkParameters )

# Component(s) in the package:
atlas_add_dictionary( TrkTrackLinkDict
                      TrkTrackLink/TrkTrackLinkDict.h
                      TrkTrackLink/selection.xml
                      LINK_LIBRARIES TrkTrackLink )
