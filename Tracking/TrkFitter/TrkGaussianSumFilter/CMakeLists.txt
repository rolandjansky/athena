# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkGaussianSumFilter )
find_package( Boost COMPONENTS container)
#Component(s) in the package:
atlas_add_component( TrkGaussianSumFilter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GeoPrimitives GaudiKernel TrkGaussianSumFilterUtilsLib
                     TrkGeometry  TrkSurfaces TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack 
                     TrkMultiComponentStateOnSurface TrkParameters TrkExInterfaces TrkExUtils TrkFitterInterfaces 
                     TrkFitterUtils TrkToolInterfaces CxxUtils TrkDetElementBase TrkCaloCluster_OnTrack 
                     TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack ${Boost_LIBRARIES} )

# Install files from the package:
atlas_install_runtime( Data/*.par )

