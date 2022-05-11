# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_RIO_OnTrack )

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

atlas_add_library( HGTD_RIO_OnTrack
                src/*.cxx
                PUBLIC_HEADERS HGTD_RIO_OnTrack
                LINK_LIBRARIES AthLinks SGTools HGTD_ReadoutGeometry Identifier
                    TrkPrepRawData HGTD_PrepRawData TrkSurfaces TrkRIO_OnTrack
                    TrkEventPrimitives)

atlas_add_dictionary( HGTD_RIO_OnTrackDict
                HGTD_RIO_OnTrack/HGTD_RIO_OnTrackDict.h
                HGTD_RIO_OnTrack/selection.xml
                LINK_LIBRARIES GaudiKernel HGTD_RIO_OnTrack)

atlas_add_test(test_HGTD_ClusterOnTrack
            SOURCES test/test_HGTD_ClusterOnTrack.cxx
            INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
            LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_RIO_OnTrack
            POST_EXEC_SCRIPT nopost.sh)
