# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir(EgammaAnalysisHelpers )

# Component(s) in the package:
atlas_add_library( EgammaAnalysisHelpersLib
  EgammaAnalysisHelpers/*.h Root/*.cxx
  PUBLIC_HEADERS EgammaAnalysisHelpers
  LINK_LIBRARIES xAODEgamma
  PRIVATE_LINK_LIBRARIES AsgMessagingLib xAODCaloEvent)

atlas_add_dictionary( EgammaAnalysisHelpersDict
  EgammaAnalysisHelpers/EgammaAnalysisHelpersDict.h
  EgammaAnalysisHelpers/selection.xml
  LINK_LIBRARIES EgammaAnalysisHelpersLib )

