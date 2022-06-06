# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( FTagAnalysisInterfaces )

# Component(s) in the package:
atlas_add_library( FTagAnalysisInterfacesLib
   FTagAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS FTagAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODJet PATCoreLib PATInterfaces CalibrationDataInterfaceLib )

atlas_add_dictionary( FTagAnalysisInterfacesDict
   FTagAnalysisInterfaces/FTagAnalysisInterfacesDict.h
   FTagAnalysisInterfaces/selection.xml
   LINK_LIBRARIES FTagAnalysisInterfacesLib )
