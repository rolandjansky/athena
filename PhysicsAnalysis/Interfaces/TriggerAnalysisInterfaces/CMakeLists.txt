# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( TriggerAnalysisInterfaces )

# Component(s) in the package:
atlas_add_library( TriggerAnalysisInterfaces
   TriggerAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TriggerAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODEgamma xAODMuon PATInterfaces )

atlas_add_dictionary( TriggerAnalysisInterfacesDict
   TriggerAnalysisInterfaces/TriggerAnalysisInterfacesDict.h
   TriggerAnalysisInterfaces/selection.xml
   LINK_LIBRARIES TriggerAnalysisInterfaces )
