# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( PMGAnalysisInterfaces )

# Component(s) in the package:
atlas_add_library( PMGAnalysisInterfacesLib
   PMGAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS PMGAnalysisInterfaces
   LINK_LIBRARIES AsgTools PATInterfaces )

atlas_add_dictionary( PMGAnalysisInterfacesDict
   PMGAnalysisInterfaces/PMGAnalysisInterfacesDict.h
   PMGAnalysisInterfaces/selection.xml
   LINK_LIBRARIES PMGAnalysisInterfacesLib )
