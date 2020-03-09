################################################################################
# Package: TBTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TBTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          TestBeam/TBEvent
                          PRIVATE
                          DetectorDescription/Identifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TBTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS TBTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} TBEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier )

atlas_add_dictionary( TBTPCnvDict
                      TBTPCnv/TBTPCnvDict.h
                      TBTPCnv/selection.xml
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TBEvent Identifier TBTPCnv )


atlas_add_test( TBADCRawContCnv_p1_test
                SOURCES
                test/TBADCRawContCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBBPCCont_p1_test
                SOURCES
                test/TBBPCContCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBEventInfoCnv_p1_test
                SOURCES
                test/TBEventInfoCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBLArDigitContainerCnv_p1_test
                SOURCES
                test/TBLArDigitContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBMWPCContCnv_p1_test
                SOURCES
                test/TBMWPCContCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBPhaseCnv_p1_test
                SOURCES
                test/TBPhaseCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBScintillatorContCnv_p1_test
                SOURCES
                test/TBScintillatorContCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBTailCatcherCnv_p1_test
                SOURCES
                test/TBTailCatcherCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBTDCCnv_p1_test
                SOURCES
                test/TBTDCCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBTDCRawContCnv_p1_test
                SOURCES
                test/TBTDCRawContCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBTrackCnv_p1_test
                SOURCES
                test/TBTrackCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBTrackInfoCnv_p1_test
                SOURCES
                test/TBTrackInfoCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )

atlas_add_test( TBTriggerPatternUnitCnv_p1_test
                SOURCES
                test/TBTriggerPatternUnitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TBTPCnv )


                    
