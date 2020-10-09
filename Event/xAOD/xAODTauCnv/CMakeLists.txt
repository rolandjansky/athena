# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTauCnv )

atlas_add_library( xAODTauCnvLib
                   xAODTauCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTauCnv
                   LINK_LIBRARIES GaudiKernel AthContainers xAODTau tauEvent )

# Component(s) in the package:
atlas_add_component( xAODTauCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTauCnvLib xAODTau tauEvent AthenaBaseComps AthenaKernel GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

