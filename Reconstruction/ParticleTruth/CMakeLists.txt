# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ParticleTruth )

# Component(s) in the package:
atlas_add_library( ParticleTruth
                   src/*.cxx
                   PUBLIC_HEADERS ParticleTruth
                   LINK_LIBRARIES AthLinks AthenaKernel GeneratorObjects Particle )

atlas_add_dictionary( ParticleTruthDict
                      ParticleTruth/ParticleTruthDict.h
                      ParticleTruth/selection.xml
                      LINK_LIBRARIES ParticleTruth
                      DATA_LINKS TrackParticleTruthCollection )

