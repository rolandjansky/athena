# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetLowBetaFinder )

# External dependencies:
find_package( ROOT COMPONENTS Minuit RIO Core Hist Minuit2 )

# Component(s) in the package:
atlas_add_component( InDetLowBetaFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib CxxUtils TRT_ConditionsData InDetIdentifier InDetLowBetaInfo InDetRIO_OnTrack TrkParameters xAODTracking TrkRIO_OnTrack TrkTrack MagFieldElements MagFieldConditions TRT_ElectronPidToolsLib )
