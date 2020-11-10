# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthAlgs )

# Component(s) in the package:
atlas_add_component( TrkTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrkTruthData xAODCore xAODTracking GeneratorObjects MCTruthClassifierLib TrkToolInterfaces )
