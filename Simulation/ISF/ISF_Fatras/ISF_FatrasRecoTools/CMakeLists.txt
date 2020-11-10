# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ISF_FatrasRecoTools )

# Component(s) in the package:
atlas_add_component( ISF_FatrasRecoTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps GeoPrimitives Identifier ISF_FatrasEvent TrkEventPrimitives TrkParameters TrkSpacePoint TrkTrackSummary TrkToolInterfaces AtlasDetDescr EventPrimitives GaudiKernel InDetIdentifier InDetPrepRawData SiSpacePoint ISF_FatrasDetDescrModel TrkDetElementBase TrkCompetingRIOsOnTrack TrkEventUtils TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkTruthData TrkTruthTrackInterfaces )
