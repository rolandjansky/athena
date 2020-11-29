# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaTools )

# Component(s) in the package:
atlas_add_component( egammaTools
   src/*.cxx
   src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps AthenaKernel CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloInterfaceLib CaloRecLib CaloUtilsLib EgammaAnalysisInterfacesLib EventPrimitives FourMom FourMomUtils GaudiKernel GeoPrimitives Identifier LArCablingLib LArRecConditions LumiBlockCompsLib PATCoreAcceptLib SGTools StoreGateLib TrkEventPrimitives egammaInterfacesLib egammaRecEvent egammaUtils xAODCaloEvent xAODEgamma xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
