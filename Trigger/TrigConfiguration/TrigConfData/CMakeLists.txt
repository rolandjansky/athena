# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfData )

# External dependencies:
find_package( Boost )

# athena library for the package:
# defines CLID for some data objects
atlas_add_library ( TrigConfData
                    TrigConfData/*.h src/*.cxx
                    PUBLIC_HEADERS TrigConfData
                    INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
                    LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel CxxUtils
                    )

# standalone library for use by detector software:
# no Athena / Gaudi dependency
atlas_add_library ( TrigConfDataSA
                    TrigConfData/*.h src/*.cxx
                    PUBLIC_HEADERS TrigConfData
                    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                    LINK_LIBRARIES ${Boost_LIBRARIES}
                    DEFINITIONS -DTRIGCONF_STANDALONE
                    )

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
