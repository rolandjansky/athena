# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaMonitoring )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Hist MathCore )
find_package( Boost )

# Component(s) in the package:
atlas_add_library(
    AthenaMonitoringLib
    src/*.cxx
    PUBLIC_HEADERS
        AthenaMonitoring
    INCLUDE_DIRS
        ${ROOT_INCLUDE_DIRS}
    PRIVATE_INCLUDE_DIRS
        ${Boost_INCLUDE_DIRS}
        ${CORAL_INCLUDE_DIRS}
    LINK_LIBRARIES
        ${ROOT_LIBRARIES}
        AthenaBaseComps
        AthenaMonitoringKernelLib
        GaudiKernel
        LumiBlockData
        TrigAnalysisInterfaces
        TrigDecisionInterface
        TrigDecisionToolLib
    PRIVATE_LINK_LIBRARIES
        ${Boost_LIBRARIES}
        ${CORAL_LIBRARIES}
        AthenaKernel
        CxxUtils
        SGAudCore
        StoreGateLib
        AthenaPoolUtilities
        EventInfo
        LWHists
        MuonAlignmentData
        InDetReadoutGeometry
        MuonAlignmentData
        TRT_ReadoutGeometry
)

atlas_add_component(
    AthenaMonitoring
    src/components/*.cxx
    INCLUDE_DIRS
        ${CORAL_INCLUDE_DIRS}
    LINK_LIBRARIES
        AthenaMonitoringLib
        LWHists
        SGAudCore
        MuonAlignmentData
)

# Install files from the package:
atlas_install_python_modules( python/*.py 
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/Run3DQTestingDriver.py share/hist_file_dump.py share/hist_diff.sh )
atlas_install_runtime( share/test_unit_ExampleMonitorAlgorithm.ref )

# Example monitor algorithm test
atlas_add_test( ExampleMonitorAlgorithm_test
    SCRIPT test/test_unit_ExampleMonitorAlgorithm.sh
    PROPERTIES TIMEOUT 300
)
