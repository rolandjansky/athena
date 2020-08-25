# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigMinBiasCnv )

atlas_add_library( xAODTrigMinBiasCnvLib
                   xAODTrigMinBiasCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigMinBiasCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigMinBias )

# Component(s) in the package:
atlas_add_component( xAODTrigMinBiasCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigCaloEvent TrigInDetEvent xAODTrigMinBiasCnvLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

