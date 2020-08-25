# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTJetHemisphereRec )

# Component(s) in the package:
atlas_add_component( TrigHLTJetHemisphereRec
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel JetEDM TrigInterfacesLib TrigParticle TrigTimeAlgsLib xAODCaloEvent xAODJet )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
