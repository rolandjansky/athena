################################################################################
# Package: AthViews
################################################################################

find_package( TBB )
find_package( GTest )
find_package( ROOT COMPONENTS Core )

# Declare the package name:
atlas_subdir( AthViews )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Trigger/TrigEvent/TrigSteeringEvent
                          Control/AthenaExamples/AthExHive
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
                          Control/AthLinks
			  AtlasTest/TestTools )

# Component(s) in the package:
atlas_add_library( AthViews
                   src/*.cxx
                   PUBLIC_HEADERS AthViews
                   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TBB_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel CxxUtils AthLinks TrigSteeringEvent )

atlas_add_component( AthViewsAlgs
                   src/*.cxx
                   src/components/*.cxx
                   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                   LINK_LIBRARIES AthViews )

atlas_add_component( AthViewsDFlow
                     src_dflow/*.cxx
                     src_dflow/components/*.cxx
                     INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES AthViews )

atlas_add_dictionary( AthViewsAlgsDict
                      AthViews/AthViewsWriteDict.h
                      AthViews/selection.xml
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel CxxUtils AthLinks TrigSteeringEvent
                      ELEMENT_LINKS TrigRoiDescriptorCollection )


atlas_add_test( ViewLinking_test 
		INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
		SOURCES test/ViewLinking_test.cxx 
		LINK_LIBRARIES ${TBB_LIBRARIES}
		AthViews TestTools )

atlas_add_test( ViewCollectionMerge_test 
    INCLUDE_DIRS ${TBB_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
    SOURCES test/ViewCollectionMerge_test.cxx 
    LINK_LIBRARIES ${TBB_LIBRARIES} ${GTEST_LIBRARIES} AthViews TestTools
    ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test")

atlas_add_test( SimpleViews SCRIPT test/test_simple_view_example.sh
                PROPERTIES PASS_REGULAR_EXPRESSION "INFO view_test running with store view_9"
                TIMEOUT 300 )

atlas_add_test( ViewScheduling SCRIPT test/test_view_scheduling.sh )

atlas_add_test( ViewInheritance SCRIPT test/test_view_inheritance.sh )

atlas_add_test( ViewNestFail SCRIPT test/test_view_nest_fail.sh
                PROPERTIES PASS_REGULAR_EXPRESSION "Execution of algorithm nest_alg failed"
                TIMEOUT 300 )

option( ATHVIEWS_DEBUG "Build debugging version" OFF )
if( ATHVIEWS_DEBUG )
  add_compile_definitions( ATHVIEWS_DEBUG )
endif()

# Install files from the package:
atlas_install_headers( AthViews )
atlas_install_joboptions( share/*.py )

