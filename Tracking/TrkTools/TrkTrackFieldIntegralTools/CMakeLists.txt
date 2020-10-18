# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrackFieldIntegralTools )

# Component(s) in the package:
atlas_add_component( TrkTrackFieldIntegralTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkDetDescrInterfaces TrkGeometry TrkMaterialOnTrack TrkMeasurementBase TrkPseudoMeasurementOnTrack TrkTrack TrkToolInterfaces )

