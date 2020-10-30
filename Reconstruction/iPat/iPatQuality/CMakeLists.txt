# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatQuality )

# Component(s) in the package:
atlas_add_component( iPatQuality
                     src/TrackQuality.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps iPatInterfaces iPatTrack )
