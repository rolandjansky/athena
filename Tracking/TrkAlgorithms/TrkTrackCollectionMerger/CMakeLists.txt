# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrackCollectionMerger )

# External dependencies:
find_package( Boost COMPONENTS unit_test_framework )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( TrkTrackCollectionMergerLib
                   src/*.cxx
                   test/PutTrackCollectionsInSG.cxx
                   PUBLIC_HEADERS TrkTrackCollectionMerger
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrkTrack TrkEventUtils TrkToolInterfaces InDetPrepRawData
                   PRIVATE_LINK_LIBRARIES TrkPrepRawData )

atlas_add_component( TrkTrackCollectionMerger
                     src/components/*.cxx
                     LINK_LIBRARIES TrkTrackCollectionMergerLib )

# Tests in the package:
atlas_add_test( TrackCollectionMerger_test
                SOURCES test/TrackCollectionMerger_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} TrkTrackCollectionMergerLib
                POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( SimpleTrackCollectionMerger_test
                SOURCES test/SimpleTrackCollectionMerger_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} TrkTrackCollectionMergerLib
                POST_EXEC_SCRIPT "nopost.sh" )

# Install files from the package:
atlas_install_joboptions( share/*.txt )
