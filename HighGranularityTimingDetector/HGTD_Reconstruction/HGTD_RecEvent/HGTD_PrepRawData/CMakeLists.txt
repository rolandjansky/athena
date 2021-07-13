# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Declare the package name:
atlas_subdir( HGTD_PrepRawData )

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

atlas_add_library( HGTD_PrepRawData
                src/*.cxx
                PUBLIC_HEADERS HGTD_PrepRawData
                LINK_LIBRARIES AthLinks SGTools HGTD_ReadoutGeometry Identifier
                    TrkPrepRawData InDetPrepRawData TrkSurfaces)

atlas_add_dictionary( HGTD_PrepRawDataDict
                HGTD_PrepRawData/HGTD_PrepRawDataDict.h
                HGTD_PrepRawData/selection.xml
                LINK_LIBRARIES GaudiKernel HGTD_PrepRawData)

atlas_add_test(test_HGTD_Cluster
            SOURCES test/test_HGTD_Cluster.cxx
            INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
            LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_PrepRawData)
