# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventBookkeeperTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( EventBookkeeperTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventBookkeeperTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib EventBookkeeperMetaData
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( EventBookkeeperTPCnvDict
                      EventBookkeeperTPCnv/EventBookkeeperTPCnvDict.h
                      EventBookkeeperTPCnv/selection.xml
                      LINK_LIBRARIES EventBookkeeperTPCnv )

# Tests in the package:
atlas_add_test( EventBookkeeperCnv_p1_test
                SOURCES
                test/EventBookkeeperCnv_p1_test.cxx
                LINK_LIBRARIES CxxUtils EventBookkeeperTPCnv TestTools )

atlas_add_test( EventBookkeeperCnv_p2_test
                SOURCES
                test/EventBookkeeperCnv_p2_test.cxx
                LINK_LIBRARIES CxxUtils EventBookkeeperTPCnv TestTools )

atlas_add_test( SkimDecisionCnv_p1_test
                SOURCES
                test/SkimDecisionCnv_p1_test.cxx
                LINK_LIBRARIES CxxUtils EventBookkeeperTPCnv TestTools )
