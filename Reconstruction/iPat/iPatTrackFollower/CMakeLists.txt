# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatTrackFollower )

# Component(s) in the package:
atlas_add_component( iPatTrackFollower
                     src/LayerAssociation.cxx
                     src/LayerPrediction.cxx
                     src/LayerPredictor.cxx
                     src/SiClusterProperties.cxx
                     src/SiliconClusterMap.cxx
                     src/SiliconLayerAssociator.cxx
                     src/TrackBuilder.cxx
                     src/TrackFollower.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetPrepRawData iPatGeometry iPatInterfaces iPatTrack TrkParameters StoreGateLib GeoPrimitives InDetConditionsSummaryService InDetIdentifier InDetReadoutGeometry InDetRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkExInterfaces TrkExUtils TrkToolInterfaces )
