# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileSimEventTPCnv )

# Component(s) in the package:
atlas_add_library( TileSimEventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS TileSimEventTPCnv
                   LINK_LIBRARIES TileSimEvent AthenaPoolCnvSvcLib )

atlas_add_dictionary( TileSimEventTPCnvDict
                      TileSimEventTPCnv/TileSimEventTPCnvDict.h
                      TileSimEventTPCnv/selection.xml
                      LINK_LIBRARIES TileSimEventTPCnv )

atlas_add_dictionary( OLD_TileSimEventTPCnvDict
                      TileSimEventTPCnv/TileSimEventTPCnvDict.h
                      TileSimEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES TileSimEventTPCnv )

# Tests in the package:
atlas_add_test( TileHitCnv_p1_test
                SOURCES
                test/TileHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES GaudiKernel TestTools TileSimEventTPCnv )

atlas_add_test( TileHitVectorCnv_p1_test
                SOURCES
                test/TileHitVectorCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES GaudiKernel TestTools TileSimEventTPCnv )
