# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1ZDC )

# Component(s) in the package:
atlas_add_component( TrigT1ZDC
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasHepMCLib GaudiKernel GeneratorObjects TrigConfInterfaces TrigConfL1Data TrigT1Interfaces )
