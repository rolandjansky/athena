# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexWeightCalculators )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist Matrix )

# Component(s) in the package:
atlas_add_component( TrkVertexWeightCalculators
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GeoPrimitives xAODTracking GaudiKernel
                     TrkVertexFitterInterfaces GeneratorObjects TrkParameters VxVertex TrkNeuralNetworkUtilsLib)

# Install files from the package:
atlas_install_runtime( share/*.root )
