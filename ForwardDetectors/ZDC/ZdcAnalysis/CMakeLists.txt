# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcAnalysis )

# Declare the package name:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Minuit Minuit2 HistPainter Graf )

# Component(s) in the package:
atlas_add_library( ZdcAnalysisLib
                   Root/*.cxx
                   PUBLIC_HEADERS ZdcAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgDataHandlesLib AsgTools xAODEventInfo xAODForward xAODTrigL1Calo
		           PRIVATE_LINK_LIBRARIES PathResolver )

atlas_add_component( ZdcAnalysis
		             src/components/*.cxx
		             LINK_LIBRARIES ZdcAnalysisLib )
