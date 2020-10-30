# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetAnalysisEDM )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetAnalysisEDMLib
                   JetAnalysisEDM/*.h
                   INTERFACE
                   PUBLIC_HEADERS JetAnalysisEDM
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent JetEvent )
