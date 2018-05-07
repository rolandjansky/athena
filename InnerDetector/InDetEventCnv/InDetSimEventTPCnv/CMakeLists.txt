################################################################################
# Package: InDetSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( InDetSimEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Generators/GeneratorObjectsTPCnv
                          InnerDetector/InDetSimEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          DetectorDescription/Identifier )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetSimEventTPCnv
                   src/InDetHits/*.cxx
                   PUBLIC_HEADERS InDetSimEventTPCnv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GaudiKernel GeneratorObjectsTPCnv InDetSimEvent AthenaPoolCnvSvcLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools Identifier )

atlas_add_dictionary( InDetSimEventTPCnvDict
                      InDetSimEventTPCnv/InDetSimEventTPCnvDict.h
                      InDetSimEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )

atlas_add_test( SiHitCnv_p1_test
                SOURCES
                test/SiHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )

atlas_add_test( SiHitCollectionCnv_p2_test
                SOURCES
                test/SiHitCollectionCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )

atlas_add_test( SiHitCollectionCnv_p3_test
                SOURCES
                test/SiHitCollectionCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )

atlas_add_test( TRT_HitCnv_p1_test
                SOURCES
                test/TRT_HitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )

atlas_add_test( TRT_HitCollectionCnv_p2_test
                SOURCES
                test/TRT_HitCollectionCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )

atlas_add_test( TRT_HitCollectionCnv_p3_test
                SOURCES
                test/TRT_HitCollectionCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )


atlas_add_test( TRT_HitCollectionCnv_p4_test
                SOURCES
                test/TRT_HitCollectionCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjectsTPCnv InDetSimEvent TestTools StoreGateLib SGtests Identifier InDetSimEventTPCnv )
