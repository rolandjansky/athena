# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ISF_FatrasRecoTools )

# Component(s) in the package:
atlas_add_library( ISF_FatrasRecoToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FatrasRecoTools
                   LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps GeoPrimitives Identifier ISF_FatrasEvent TrkEventPrimitives TrkParameters TrkSpacePoint TrkTrackSummary TrkToolInterfaces AtlasDetDescr EventPrimitives GaudiKernel InDetIdentifier InDetPrepRawData SiSpacePoint ISF_FatrasDetDescrModel TrkDetElementBase TrkCompetingRIOsOnTrack TrkEventUtils TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkTruthData TrkTruthTrackInterfaces )

# Component(s) in the package:
atlas_add_component( ISF_FatrasRecoTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ISF_FatrasRecoToolsLib )

# Test(s) in the package:
atlas_add_test( SiSpacePointMakerTool_test
                SOURCES test/SiSpacePointMakerTool_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AthenaKernel IdDictParser TrkGeometry ISF_FatrasRecoToolsLib TestTools 
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
