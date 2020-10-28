# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAlignTrkInfo )

# Component(s) in the package:
atlas_add_library( InDetAlignTrkInfo
                   src/AlignTrk.cxx
                   src/AlignTrkContainer.cxx
                   src/AlignSiHit.cxx
                   src/AlignTRTHit.cxx
                   PUBLIC_HEADERS InDetAlignTrkInfo
                   LINK_LIBRARIES AthContainers AthenaKernel )
