# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( METInterface )

# Component(s) in the package:
atlas_add_library( METInterface
   METInterface/*.h
   INTERFACE
   PUBLIC_HEADERS METInterface
   LINK_LIBRARIES AsgTools xAODBase xAODEventInfo xAODJet xAODMissingET
   PATInterfaces )

atlas_add_dictionary( METInterfaceDict
   METInterface/METInterfaceDict.h
   METInterface/selection.xml
   LINK_LIBRARIES METInterface )
