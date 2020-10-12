# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODJetCnv )

atlas_add_library( xAODJetCnvLib
                   xAODJetCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODJetCnv
                   LINK_LIBRARIES GaudiKernel xAODJet xAODBase AthLinks )

# Component(s) in the package:
atlas_add_component( xAODJetCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODJetCnvLib xAODJet GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests JetEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

