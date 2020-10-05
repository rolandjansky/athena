# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigAnalysisInterfaces )

# Component(s) in the package:
atlas_add_library( TrigAnalysisInterfaces
   TrigAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TrigAnalysisInterfaces
   LINK_LIBRARIES AsgTools )

atlas_add_dictionary( TrigAnalysisInterfacesDict
   TrigAnalysisInterfaces/TrigAnalysisInterfacesDict.h
   TrigAnalysisInterfaces/selection.xml
   LINK_LIBRARIES TrigAnalysisInterfaces )
