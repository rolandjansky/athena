# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthViews )

# External package dependencies:
find_package( TBB )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( AthViews
                   src/*.cxx
                   PUBLIC_HEADERS AthViews
                   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TBB_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib TrigSteeringEvent SGTools )

atlas_add_component( AthViewsAlgs
                     src/components/*.cxx
                     LINK_LIBRARIES AthViews )

atlas_add_component( AthViewsDFlow
                     src_dflow/*.cxx
                     src_dflow/components/*.cxx
                     LINK_LIBRARIES AthViews AthExHiveLib )

atlas_add_dictionary( AthViewsAlgsDict
                      AthViews/AthViewsWriteDict.h
                      AthViews/selection.xml
                      LINK_LIBRARIES AthLinks TrigSteeringEvent
                      ELEMENT_LINKS TrigRoiDescriptorCollection )

option( ATHVIEWS_DEBUG "Build debugging version" OFF )
if( ATHVIEWS_DEBUG )
  add_compile_definitions( ATHVIEWS_DEBUG )
endif()

# Tests in the package:
atlas_add_test( ViewLinking_test
                SOURCES test/ViewLinking_test.cxx
                LINK_LIBRARIES AthViews TestTools )

atlas_add_test( ViewCollectionMerge_test
                SOURCES test/ViewCollectionMerge_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GTEST_LIBRARIES} AthViews CxxUtils
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test"
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( SimpleViews
                SCRIPT athena.py --threads=1 AthViews/SimpleViewExample.py
                PROPERTIES PASS_REGULAR_EXPRESSION "INFO view_test running with store view_9"
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( ViewScheduling
                SCRIPT athena.py --threads=1 AthViews/ViewScheduling.py
                LOG_SELECT_PATTERN "INFO condTestAlg running with store view_0|INFO 109" )

atlas_add_test( ViewInheritance
                SCRIPT athena.py --threads=1 AthViews/ViewInheritance.py
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( ViewNestFail
                SCRIPT athena.py --threads=1 AthViews/ViewNest.py
                PROPERTIES PASS_REGULAR_EXPRESSION "Execution of algorithm nest_alg failed"
                POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_joboptions( share/*.py )
