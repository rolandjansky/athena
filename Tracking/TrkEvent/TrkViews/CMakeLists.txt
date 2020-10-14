# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkViews )

# Component(s) in the package:
atlas_add_library( TrkViewsLib
                   TrkViews/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkViews
                   LINK_LIBRARIES AthContainers TrkTrack xAODCore )

atlas_add_dictionary( TrkViewsDict
                      TrkViews/TrkViewsDict.h
                      TrkViews/selection.xml
                      LINK_LIBRARIES TrkViewsLib )
