# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkCaloExtension )

# Component(s) in the package:
atlas_add_library( TrkCaloExtension
                   src/*.cxx
                   PUBLIC_HEADERS TrkCaloExtension
                   LINK_LIBRARIES AthContainers GeoPrimitives xAODCore TrkParameters TrkParametersIdentificationHelpers )
