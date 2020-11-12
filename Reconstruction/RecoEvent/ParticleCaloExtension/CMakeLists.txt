# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ParticleCaloExtension )

# Component(s) in the package:
atlas_add_library( ParticleCaloExtension
                   src/*.cxx
                   PUBLIC_HEADERS ParticleCaloExtension
                   LINK_LIBRARIES CaloEvent AthContainers xAODCaloEvent xAODCore TrkCaloExtension )
