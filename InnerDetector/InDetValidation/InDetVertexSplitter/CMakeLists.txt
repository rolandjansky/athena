# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetVertexSplitter )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree Minuit )

# Component(s) in the package:
atlas_add_component( InDetVertexSplitter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODEventInfo GaudiKernel GeneratorObjects Particle TrkParticleBase TrkTrack VxVertex )

# Install files from the package:
atlas_install_joboptions( share/*.py )

