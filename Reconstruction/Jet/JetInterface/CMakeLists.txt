# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetInterface )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_library( JetInterface
   JetInterface/*.h Root/*.cxx
   PUBLIC_HEADERS JetInterface
   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
   LINK_LIBRARIES ${FASTJET_LIBRARIES} AsgTools AsgDataHandlesLib xAODEventInfo xAODJet xAODTracking xAODBase )

atlas_add_dictionary( JetInterfaceDict
   JetInterface/JetInterfaceDict.h
   JetInterface/selection.xml
   LINK_LIBRARIES JetInterface )
