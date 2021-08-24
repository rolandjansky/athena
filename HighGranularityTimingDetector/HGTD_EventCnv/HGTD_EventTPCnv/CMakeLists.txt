################################################################################
#Package : HGTD_EventTPCnv
################################################################################

#Declare the package name:
atlas_subdir( HGTD_EventTPCnv )

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

##without this the headers in the subfolder are not found!!
atlas_depends_on_subdirs(
            PUBLIC InnerDetector/InDetEventCnv/InDetEventTPCnv
)

#Component(s) in the package:
atlas_add_tpcnv_library( HGTD_EventTPCnv
                         HGTD_EventTPCnv/*.h src/*.cxx
                         PUBLIC_HEADERS HGTD_EventTPCnv
                         LINK_LIBRARIES DataModel DataModelAthenaPoolLib StoreGateLib
                           AthenaPoolCnvSvcLib AthenaPoolUtilities Identifier GaudiKernel
                           HGTD_Identifier TrkEventTPCnv HGTD_PrepRawData HGTD_RawData
                         PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel TrkMeasurementBase
                           EventPrimitives HGTD_ReadoutGeometry TrkEventPrimitives CxxUtils
                           InDetEventTPCnv)

atlas_add_dictionary( HGTD_EventTPCnvDict
                      HGTD_EventTPCnv/HGTD_EventTPCnvDict.h HGTD_EventTPCnv/selection.xml
                      LINK_LIBRARIES HGTD_EventTPCnv )

set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

atlas_add_test( HGTD_ClusterCnv_p1_test
    SOURCES test/HGTD_ClusterCnv_p1_test.cxx
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_EventTPCnv IdDictParser)

atlas_add_test( HGTD_ClusterContainerCnv_p1_test
    SOURCES test/HGTD_ClusterContainerCnv_p1_test.cxx
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_EventTPCnv TestTools IdDictParser SGTools
      GaudiKernel
    ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}")

atlas_add_test( HGTD_RDOCnv_p1_test
    SOURCES test/HGTD_RDOCnv_p1_test.cxx
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_EventTPCnv IdDictParser)

atlas_add_test( HGTD_RDOContainerCnv_p1_test
    SOURCES test/HGTD_RDOContainerCnv_p1_test.cxx
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_EventTPCnv IdDictParser TestTools
    SGTools GaudiKernel
    ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}")
