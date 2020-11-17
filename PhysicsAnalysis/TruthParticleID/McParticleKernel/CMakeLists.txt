# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( McParticleKernel )

# Component(s) in the package:
atlas_add_library( McParticleKernel
                   src/*.cxx
                   PUBLIC_HEADERS McParticleKernel
                   LINK_LIBRARIES GaudiKernel AtlasHepMCLib )

