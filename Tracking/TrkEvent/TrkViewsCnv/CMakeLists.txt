# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkViewsCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkViewsCnvPoolCnv
                           FILES TrkViews/TrackViewVector.h
                           INCLUDE_DIRS TrkViews
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib TrkTrack TrkViewsLib )
