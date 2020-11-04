# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventShapeInterface )

# Component(s) in the package:
atlas_add_library( EventShapeInterface
   EventShapeInterface/*.h
   INTERFACE
   PUBLIC_HEADERS EventShapeInterface
   LINK_LIBRARIES AsgTools xAODBase xAODEventShape )

atlas_add_dictionary( EventShapeInterfaceDict
   EventShapeInterface/EventShapeInterfaceDict.h
   EventShapeInterface/selection.xml
   LINK_LIBRARIES EventShapeInterface )
