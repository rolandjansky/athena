# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfData )

# Extra dependencies, based on the environment:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthenaKernel )
endif()

# External dependencies:
find_package( Boost )
find_package( CORAL QUIET COMPONENTS RelationalAccess )

# Component(s) in the package:
atlas_add_library ( TrigConfData
                    TrigConfData/*.h src/*.cxx
                    PUBLIC_HEADERS TrigConfData
                    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                    LINK_LIBRARIES ${Boost_LIBRARIES} ${extra_libs} CxxUtils )

# Tests in the package:
atlas_add_test( ConstIter SOURCES test/itertest.cxx
                LINK_LIBRARIES TrigConfData
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TestLogicParser SOURCES test/testLogicParser.cxx
                LINK_LIBRARIES TrigConfData
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TestFileRW SOURCES test/testfilerw.cxx
                LINK_LIBRARIES TrigConfData
                ENVIRONMENT "TESTFILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/test/"
                POST_EXEC_SCRIPT nopost.sh )

if( NOT XAOD_ANALYSIS )
# test excluded from AnalysisBase and AthAnalysis where TrigConfIO is absent
atlas_add_test( TestDoubleMenuLoad SOURCES test/doubleLoad.cxx
                INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${CORAL_LIBRARIES} TrigConfData TrigConfIO
                ENVIRONMENT "CLArgs=TRIGGERDBDEV1 45 179 152 4"
                PRE_EXEC_SCRIPT "rm -f *1.json *2.json NoDBConnection.txt"
                POST_EXEC_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/share/doubleTestComp.py )
endif()
