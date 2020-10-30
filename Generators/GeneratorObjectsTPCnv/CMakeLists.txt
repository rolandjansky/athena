# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeneratorObjectsTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_tpcnv_library( GeneratorObjectsTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS GeneratorObjectsTPCnv
                         PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         LINK_LIBRARIES AthenaPoolCnvSvcLib AtlasHepMCLib GaudiKernel GeneratorObjects
                         PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaKernel GenInterfacesLib StoreGateLib TestTools)

atlas_add_dictionary( GeneratorObjectsTPCnvDict
                      GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
                      GeneratorObjectsTPCnv/selection.xml
                      LINK_LIBRARIES GeneratorObjectsTPCnv )

atlas_add_dictionary( OLD_GeneratorObjectsTPCnvDict
                      GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
                      GeneratorObjectsTPCnv/OLD_selection.xml
                      LINK_LIBRARIES GeneratorObjectsTPCnv )

atlas_add_test( McEventCollectionCnv_p4_test
                SOURCES
                test/McEventCollectionCnv_p4_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} GeneratorObjectsTPCnv )

atlas_add_test( McEventCollectionCnv_p5_test
                SOURCES
                test/McEventCollectionCnv_p5_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} GeneratorObjectsTPCnv )

atlas_add_test( HepMcParticleLinkCnv_p1_test
                SOURCES
                test/HepMcParticleLinkCnv_p1_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} GeneratorObjectsTPCnv StoreGateLib TestTools )

atlas_add_test( HepMcParticleLinkCnv_p2_test
                SOURCES
                test/HepMcParticleLinkCnv_p2_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} GeneratorObjectsTPCnv StoreGateLib TestTools )
