# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODEgammaCnv )

# Component(s) in the package:
atlas_add_library( xAODEgammaCnvLib
                   xAODEgammaCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODEgammaCnv
                   LINK_LIBRARIES AthContainers GaudiKernel xAODEgamma )

atlas_add_component( xAODEgammaCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaBaseComps AthenaKernel FourMom GaudiKernel Particle ParticleEvent egammaEvent xAODEgamma xAODTracking xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

