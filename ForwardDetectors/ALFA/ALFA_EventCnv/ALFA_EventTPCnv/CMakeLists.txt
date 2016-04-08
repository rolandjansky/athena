################################################################################
# Package: ALFA_EventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_EventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          ForwardDetectors/ALFA/ALFA_CLinkEv
                          ForwardDetectors/ALFA/ALFA_RawEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_GloRecEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecCorrEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecEv
                          ForwardDetectors/ALFA/ALFA_SimEv
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ALFA_EventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_EventTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv DataModelAthenaPoolLib AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES TestTools CxxUtils Identifier GaudiKernel )

atlas_add_dictionary( ALFA_EventTPCnvDict
                      ALFA_EventTPCnv/ALFA_EventTPCnvDict.h
                      ALFA_EventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_CLinkEventCnv_p1_test
                SOURCES
                test/ALFA_CLinkEventCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_DigitCnv_p1_test
                SOURCES
                test/ALFA_DigitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_DigitCollectionCnv_p1_test
                SOURCES
                test/ALFA_DigitCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_ODDigitCnv_p1_test
                SOURCES
                test/ALFA_ODDigitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_ODDigitCollectionCnv_p1_test
                SOURCES
                test/ALFA_ODDigitCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_GloRecEventCnv_p1_test
                SOURCES
                test/ALFA_GloRecEventCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_GloRecEvCollectionCnv_p1_test
                SOURCES
                test/ALFA_GloRecEvCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_HitCnv_p1_test
                SOURCES
                test/ALFA_HitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_HitCollectionCnv_p1_test
                SOURCES
                test/ALFA_HitCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_ODHitCnv_p1_test
                SOURCES
                test/ALFA_ODHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_ODHitCollectionCnv_p1_test
                SOURCES
                test/ALFA_ODHitCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecCorrEventCnv_p1_test
                SOURCES
                test/ALFA_LocRecCorrEventCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecCorrEvCollectionCnv_p1_test
                SOURCES
                test/ALFA_LocRecCorrEvCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecCorrODEventCnv_p1_test
                SOURCES
                test/ALFA_LocRecCorrODEventCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecCorrODEvCollectionCnv_p1_test
                SOURCES
                test/ALFA_LocRecCorrODEvCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecEventCnv_p1_test
                SOURCES
                test/ALFA_LocRecEventCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecEvCollectionCnv_p1_test
                SOURCES
                test/ALFA_LocRecEvCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecODEventCnv_p1_test
                SOURCES
                test/ALFA_LocRecODEventCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_LocRecODEvCollectionCnv_p1_test
                SOURCES
                test/ALFA_LocRecODEvCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_RawDataCnv_p1_test
                SOURCES
                test/ALFA_RawDataCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_RawDataCnv_charge_p1_test
                SOURCES
                test/ALFA_RawDataCnv_charge_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_RawDataCollectionCnv_p1_test
                SOURCES
                test/ALFA_RawDataCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_RawDataCollectionCnv_charge_p1_test
                SOURCES
                test/ALFA_RawDataCollectionCnv_charge_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_RawDataContainerCnv_p1_test
                SOURCES
                test/ALFA_RawDataContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

atlas_add_test( ALFA_RawDataContainerCnv_charge_p1_test
                SOURCES
                test/ALFA_RawDataContainerCnv_charge_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv TestTools CxxUtils Identifier GaudiKernel ALFA_EventTPCnv )

