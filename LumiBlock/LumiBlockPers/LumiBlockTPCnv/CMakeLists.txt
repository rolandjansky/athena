# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LumiBlockTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( LumiBlockTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LumiBlockTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib LumiBlockData
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( LumiBlockTPCnvDict
                      LumiBlockTPCnv/LumiBlockTPCnvDict.h
                      LumiBlockTPCnv/selection.xml
                      LINK_LIBRARIES LumiBlockTPCnv )

# Tests in the package:
atlas_add_test( LumiBlockRangeCnv_p1_test
                SOURCES
                test/LumiBlockRangeCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel TestTools LumiBlockTPCnv )

atlas_add_test( LumiBlockRangeCnv_p2_test
                SOURCES
                test/LumiBlockRangeCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel TestTools LumiBlockTPCnv )
