# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( JetAnalysisInterfaces )

# Component(s) in the package:
atlas_add_library( JetAnalysisInterfacesLib
   JetAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS JetAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODJet PATCoreLib PATInterfaces )

atlas_add_dictionary( JetAnalysisInterfacesDict
   JetAnalysisInterfaces/JetAnalysisInterfacesDict.h
   JetAnalysisInterfaces/selection.xml
   LINK_LIBRARIES JetAnalysisInterfacesLib )
