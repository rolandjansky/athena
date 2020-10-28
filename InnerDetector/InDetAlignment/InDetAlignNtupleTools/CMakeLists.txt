# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAlignNtupleTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetAlignNtupleTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel EventPrimitives xAODEventInfo InDetIdentifier TrkAlignEvent TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData TrkVertexOnTrack TrkExInterfaces TrkToolInterfaces TrkAlignInterfaces )
