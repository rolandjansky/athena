# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_Rec )

# Component(s) in the package:
atlas_add_component( TRT_Rec
                     src/TRT_Recon.cxx
                     src/TRT_DataSelector.cxx
                     src/TRT_Histogrammer.cxx
                     src/TRT_Fit.cxx
                     src/TRT_Prediction.cxx
                     src/TRT_Predictor.cxx
                     src/TRT_RoadData.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GeoPrimitives IRegionSelector Identifier GaudiKernel iPatInterfaces RoiDescriptor InDetIdentifier TRT_ReadoutGeometry InDetPrepRawData iPatTrack iPatTrackParameters TrkGeometry TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkExInterfaces TrkExUtils TrkToolInterfaces )
