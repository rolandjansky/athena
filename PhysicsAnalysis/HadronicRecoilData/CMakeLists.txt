# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HadronicRecoilData )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_library( HadronicRecoilData
                   src/*.cxx
                   PUBLIC_HEADERS HadronicRecoilData
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent AthContainers AthenaKernel McParticleEvent muonEvent Particle egammaEvent )
