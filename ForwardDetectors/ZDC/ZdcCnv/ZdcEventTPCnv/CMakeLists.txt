################################################################################
# Package: ZdcEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( ZdcEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          ForwardDetectors/ZDC/ZDC_SimEvent
                          ForwardDetectors/ZDC/ZdcEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ZdcEventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS ZdcEventTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ZDC_SimEvent ZdcEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES TestTools CxxUtils Identifier GaudiKernel )

atlas_add_dictionary( ZdcEventTPCnvDict
                      ZdcEventTPCnv/ZdcEventTPCnvDict.h
                      ZdcEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZdcDigitsCnv_p1_test
                SOURCES
                test/ZdcDigitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZdcDigitsCollectionCnv_p1_test
                SOURCES
                test/ZdcDigitsCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZdcRawChannelCnv_p1_test
                SOURCES
                test/ZdcRawChannelCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZdcRawChannelCollectionCnv_p1_test
                SOURCES
                test/ZdcRawChannelCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZDC_SimPixelHitCnv_p1_test
                SOURCES
                test/ZDC_SimPixelHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZDC_SimPixelHit_CollectionCnv_p1_test
                SOURCES
                test/ZDC_SimPixelHit_CollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZDC_SimStripHitCnv_p1_test
                SOURCES
                test/ZDC_SimStripHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

atlas_add_test( ZDC_SimStripHit_CollectionCnv_p1_test
                SOURCES
                test/ZDC_SimStripHit_CollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent TestTools CxxUtils Identifier GaudiKernel ZdcEventTPCnv )

