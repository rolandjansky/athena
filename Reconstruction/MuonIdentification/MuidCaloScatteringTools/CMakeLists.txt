# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuidCaloScatteringTools )

# Component(s) in the package:
atlas_add_component( MuidCaloScatteringTools
                     src/MuidCaloMaterialParam.cxx
                     src/MuidCaloTrackStateOnSurface.cxx
                     src/MuidMaterialEffectsOnTrackProvider.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuidInterfaces TrkDetDescrInterfaces TrkSurfaces TrkParameters FourMomUtils
			            TrkExInterfaces GeoPrimitives MuidEvent muonEvent TrkGeometry TrkMaterialOnTrack TrkTrack TrkExUtils MagFieldConditions )
