# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigTruthAlgs )

# Component(s) in the package:
atlas_add_component( InDetTrigTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel InDetSimData TrkTruthData TrigInterfacesLib AthLinks InDetPrepRawData Particle ParticleTruth TrkTrack TrkToolInterfaces InDetTruthInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
