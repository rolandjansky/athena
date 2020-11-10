# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatTrackFinder )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( iPatTrackFinder
                     src/AssignedHits.cxx
                     src/CandidateBuilder.cxx
                     src/CombinationMaker.cxx
                     src/FinderTolerances.cxx
                     src/Point.cxx
                     src/PointGroup.cxx
                     src/PointManager.cxx
                     src/PrimaryCandidate.cxx
                     src/SecondaryCandidate.cxx
                     src/TrackFinder.cxx
                     src/TrackManager.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps Identifier EventPrimitives GaudiKernel InDetPrepRawData iPatInterfaces iPatTrack InDetReadoutGeometry PixelReadoutGeometry MagFieldConditions iPatGeometry iPatTrackParameters iPatUtility TrkPrepRawData TrkSpacePoint VxVertex StoreGateLib BeamSpotConditionsData )
