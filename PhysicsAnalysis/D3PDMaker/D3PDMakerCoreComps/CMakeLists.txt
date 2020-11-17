# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( D3PDMakerCoreComps )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_component( D3PDMakerCoreComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel AthenaPoolCnvSvcLib D3PDMakerInterfaces D3PDMakerUtils GaudiKernel ParticleEvent RootUtils SGTools )

# Install files from the package:
atlas_install_python_modules( python/*.py )

