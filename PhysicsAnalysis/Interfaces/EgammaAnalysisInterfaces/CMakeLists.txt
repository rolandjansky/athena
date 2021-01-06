# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( EgammaAnalysisInterfaces )

# Component(s) in the package:
atlas_add_library( EgammaAnalysisInterfacesLib
   EgammaAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS EgammaAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma xAODTracking PATInterfaces PATCoreLib )

atlas_add_dictionary( EgammaAnalysisInterfacesDict
   EgammaAnalysisInterfaces/EgammaAnalysisInterfacesDict.h
   EgammaAnalysisInterfaces/selection.xml
   LINK_LIBRARIES EgammaAnalysisInterfacesLib )

