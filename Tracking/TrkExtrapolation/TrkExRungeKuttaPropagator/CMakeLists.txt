# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExRungeKuttaPropagator )

# Component(s) in the package:
atlas_add_component( TrkExRungeKuttaPropagator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkEventPrimitives TrkNeutralParameters
                     TrkParameters TrkExInterfaces TrkGeometry TrkSurfaces TrkPatternParameters TrkExUtils
                     MagFieldElements MagFieldConditions CxxUtils )
