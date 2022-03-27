# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MdtDriftCircleOnTrackCreator )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( MdtDriftCircleOnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps GaudiKernel MuonRIO_OnTrack MuonRecToolInterfaces TrkEventPrimitives TrkSpaceTimePoint Identifier EventPrimitives MdtCalibData MdtCalibSvcLib MuonCalibEvent MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData TrkDistortedSurfaces TrkSurfaces TrkParameters )

