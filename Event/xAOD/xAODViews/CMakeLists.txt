# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODViews )

# Component(s) in the package.
atlas_add_library( xAODViewsLib
   xAODViews/*.h
   INTERFACE
   PUBLIC_HEADERS xAODViews
   LINK_LIBRARIES AthContainers xAODCore xAODTracking )

atlas_add_dictionary( xAODViewsDict
   xAODViews/xAODViewsDict.h
   xAODViews/selection.xml
   LINK_LIBRARIES xAODViewsLib )
