################################################################################
#Package : HGTD_RawData
################################################################################

#Declare the package name:
atlas_subdir( HGTD_RawData )

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

atlas_depends_on_subdirs( PUBLIC
                          Event/EventContainers) ##why t.f. is this necessary???

atlas_add_library( HGTD_RawData
                src/*.cxx
                PUBLIC_HEADERS HGTD_RawData EventContainers
                LINK_LIBRARIES Identifier AthContainers SGTools)

#atlas_add_dictionary( HGTD_PrepRawDataDict
#                HGTD_RawData/HGTD_PrepRawDataDict.h
#                HGTD_RawData/selection.xml
#                LINK_LIBRARIES GaudiKernel HGTD_RawData)

set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

atlas_add_test(test_HGTD_RDO
           SOURCES test/test_HGTD_RDO.cxx
           INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
           LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_RawData)

atlas_add_test(test_HGTD_RDOColl
          SOURCES test/test_HGTD_RDOColl.cxx
          INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
          LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_RawData)

atlas_add_test(test_HGTD_RDOCont
          SOURCES test/test_HGTD_RDOCont.cxx
          INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
          LINK_LIBRARIES ${Boost_LIBRARIES} HGTD_RawData TestTools SGTools
          GaudiKernel StoreGateLib
          ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}")
