# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DetDescrCondTPCnv )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_tpcnv_library( DetDescrCondTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS DetDescrCondTPCnv
                         PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES LINK_LIBRARIES AthenaPoolCnvSvcLib DetDescrConditions Identifier RootConversions
                         PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )

atlas_add_dictionary( DetDescrCondTPCnvDict
                      DetDescrCondTPCnv/DetDescrCondTPCnvDict.h
                      DetDescrCondTPCnv/selection.xml
                      LINK_LIBRARIES DetDescrCondTPCnv )

# Test in the package:
atlas_add_test( DetStatusMapCnv_p1_test
                SOURCES
                test/DetStatusMapCnv_p1_test.cxx
                LINK_LIBRARIES DetDescrCondTPCnv GaudiKernel TestTools )

atlas_add_test( AlignableTransformCnv_p1_test
                SOURCES
                test/AlignableTransformCnv_p1_test.cxx
                LINK_LIBRARIES DetDescrCondTPCnv GaudiKernel TestTools )

atlas_add_test( DetCondCFloatCnv_p1_test
                SOURCES
                test/DetCondCFloatCnv_p1_test.cxx
                LINK_LIBRARIES DetDescrCondTPCnv GaudiKernel TestTools )

atlas_add_test( DetDescrCondIdentifier_old_cnv_test
                SOURCES
                test/DetDescrCondIdentifier_old_cnv_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils DetDescrCondTPCnv TestTools )

atlas_add_test( DetCondKeyTransCnv_p1_test
                SOURCES
                test/DetCondKeyTransCnv_p1_test.cxx
                LINK_LIBRARIES DetDescrCondTPCnv GaudiKernel TestTools )
