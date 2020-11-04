# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetDNASeparator )

# Component(s) in the package:
atlas_add_component( InDetDNASeparator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkMaterialOnTrack TrkParameters TrkFitterInterfaces TrkFitterUtils EventPrimitives GaudiKernel InDetIdentifier TrkSurfaces TrkMeasurementBase TrkTrack )
