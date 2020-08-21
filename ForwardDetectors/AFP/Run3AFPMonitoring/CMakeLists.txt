################################################################################
# Package: Run3AFPMonitoring
################################################################################

# Declare the package name:
atlas_subdir( Run3AFPMonitoring )


# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        Control/AthenaBaseComps
        Control/AthenaMonitoringKernel
        GaudiKernel
        Event/xAOD/xAODForward
        xAODForward/AFPSiHit
        xAODForward/AFPToFHit
        LumiBlock/LumiBlockComps
        LumiBlock/LumiBlockData
        Trigger/TrigEvent/TrigDecisionInterface
    PRIVATE
        Control/AthenaMonitoring
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
        MuonSpectrometer/MuonAlignment/MuonAlignmentData
)

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( Run3AFPMonitoring
    Run3AFPMonitoring/*.h src/*.cxx src/components/*.cxx
    INCLUDE_DIRS
        ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES
        ${ROOT_LIBRARIES}
        AthenaBaseComps
        AthenaMonitoringLib
        AthenaMonitoringKernelLib
        GaudiKernel
        LumiBlockCompsLib
        LumiBlockData
        TrigDecisionToolLib
#        TrigAnalysisInterfaces
        xAODForward
        #xAODTrigger
        #Trigger
        AthenaKernel
        SGAudCore
        AthenaPoolUtilities
        EventInfo
        LWHists
)

atlas_install_python_modules( python/*.py )

