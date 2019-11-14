################################################################################
# Package: AthenaMonitoring
################################################################################

# Declare the package name:
atlas_subdir( AthenaMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        Control/AthenaBaseComps
        Control/AthenaMonitoringKernel
        GaudiKernel
        LumiBlock/LumiBlockComps
        LumiBlock/LumiBlockData
        Trigger/TrigEvent/TrigDecisionInterface
    PRIVATE
        AtlasTest/TestTools
        Control/AthenaKernel
        Control/CxxUtils
        Control/SGMon/SGAudCore
        Database/AthenaPOOL/AthenaPoolUtilities
        Event/xAOD/xAODEventInfo
        Event/EventInfo
        Tools/LWHists
        Trigger/TrigAnalysis/TrigDecisionTool
        Trigger/TrigAnalysis/TrigAnalysisInterfaces
)

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Boost )
find_package( ROOT COMPONENTS Core )

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
        ${Boost_LIBRARIES}
        ${ROOT_LIBRARIES}
        AthenaBaseComps
        AthenaMonitoringKernelLib
        GaudiKernel
        LumiBlockCompsLib
        LumiBlockData
        TrigDecisionToolLib
    PRIVATE_LINK_LIBRARIES
        ${CORAL_LIBRARIES}
        AthenaKernel
        SGAudCore
        AthenaPoolUtilities
        EventInfo
        LWHists
)

atlas_add_component(
    AthenaMonitoring
    src/components/*.cxx
    INCLUDE_DIRS
        ${CORAL_INCLUDE_DIRS}
    LINK_LIBRARIES
        AthenaMonitoringLib
        AthenaMonitoringKernelLib
        LumiBlockData
        LWHists
        SGAudCore
        TrigDecisionToolLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py 
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/Run3DQTestingDriver.py share/hist_file_dump.py share/hist_diff.sh )
