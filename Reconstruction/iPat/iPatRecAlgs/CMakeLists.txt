# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatRecAlgs )

# Component(s) in the package:
atlas_add_component( iPatRecAlgs
                     src/iPatRec.cxx
                     src/iPatShortTracks.cxx
                     src/iPatStatistics.cxx
                     src/iPatTrackTruthAssociator.cxx
                     src/IntersectorTest.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel iPatRecEvent iPatTrack GeoPrimitives xAODEventInfo InDetPrepRawData iPatInterfaces iPatTrackParameters iPatUtility TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkSpacePoint TrkTrack TrkTruthData TrkExInterfaces TrkExUtils TrkFitterInterfaces GenInterfacesLib )
