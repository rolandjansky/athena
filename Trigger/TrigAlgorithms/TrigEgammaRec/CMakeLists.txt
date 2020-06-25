# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaRec )

# Component(s) in the package:
atlas_add_component( TrigEgammaRec
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES CaloDetDescrLib CaloEvent EgammaAnalysisInterfacesLib GaudiKernel LumiBlockCompsLib RecoToolInterfaces TrigInterfacesLib TrigParticle TrigSteeringEvent TrigTimeAlgsLib egammaEvent egammaInterfacesLib egammaRecEvent xAODCaloEvent xAODEgamma xAODEgammaCnvLib xAODEventShape xAODPrimitives xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
