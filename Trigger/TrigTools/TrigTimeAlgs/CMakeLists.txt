# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTimeAlgs )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_library( TrigTimeAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTimeAlgs
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils )

atlas_add_component( TrigTimeAlgs
                     src/components/*.cxx
                     LINK_LIBRARIES TrigTimeAlgsLib )

# Tests:
atlas_add_test( TimerSvc_test
                SOURCES
                test/TimerSvc_test.cxx
                LINK_LIBRARIES LINK_LIBRARIES TestTools TrigTimeAlgsLib
                POST_EXEC_SCRIPT nopost.sh
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
