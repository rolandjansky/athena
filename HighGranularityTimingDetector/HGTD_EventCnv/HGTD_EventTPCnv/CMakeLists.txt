# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Declare the package name:
atlas_subdir( HGTD_EventTPCnv )

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

#Component(s) in the package:
atlas_add_tpcnv_library( HGTD_EventTPCnv
                         HGTD_EventTPCnv/*.h src/*.cxx
                         PUBLIC_HEADERS HGTD_EventTPCnv
                         LINK_LIBRARIES AthLinks DataModelAthenaPoolLib StoreGateLib
                           AthenaPoolCnvSvcLib AthenaPoolUtilities Identifier GaudiKernel
                           HGTD_Identifier TrkEventTPCnv HGTD_PrepRawData InDetEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel TrkMeasurementBase
                           EventPrimitives HGTD_ReadoutGeometry TrkEventPrimitives)

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
