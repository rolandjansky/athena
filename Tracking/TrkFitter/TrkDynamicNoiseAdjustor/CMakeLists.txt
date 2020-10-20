# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDynamicNoiseAdjustor )

# Component(s) in the package:
atlas_add_component( TrkDynamicNoiseAdjustor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkFitterInterfaces EventPrimitives TrkDetElementBase TrkGeometry TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkParameters TrkExInterfaces TrkFitterUtils TrkToolInterfaces )
