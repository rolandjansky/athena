# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( AsgAnalysisInterfaces )

# External(s):
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AsgAnalysisInterfaces
   AsgAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS AsgAnalysisInterfaces
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODEventInfo PATInterfaces )

atlas_add_dictionary( AsgAnalysisInterfacesDict
   AsgAnalysisInterfaces/AsgAnalysisInterfacesDict.h
   AsgAnalysisInterfaces/selection.xml
   LINK_LIBRARIES AsgAnalysisInterfaces )

