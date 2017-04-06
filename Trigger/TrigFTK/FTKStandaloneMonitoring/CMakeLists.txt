atlas_subdir(FTKStandaloneMonitoring)
#include(ATLAS)
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamData
                          Trigger/TrigFTK/TrigFTK_RawData
			  Trigger/TrigAnalysis/TrigInDetAnalysis
			  Trigger/TrigAnalysis/TrigInDetAnalysisUtils	
                          PRIVATE
                          Trigger/TrigFTK/TrigFTKSim
                          )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write ROOT) 

atlas_add_library(FTKStandaloneMonitoringLib
                  src/*.cxx
		  components/*.cxx 
                  PUBLIC_HEADERS FTKStandaloneMonitoring
                  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                  LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}   ByteStreamData   TrigFTK_RawData TrigFTKSimLib TrigInDetAnalysisUtils TrigInDetAnalysis TrigFTK_RawData
                  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )
# Component(s) in the package:
atlas_add_component( FTKStandaloneMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}   ByteStreamData TrigFTK_RawData TrigFTKSimLib TrigInDetAnalysisUtils TrigInDetAnalysis TrigFTK_RawData)


atlas_add_executable(MonitoringFTKHWSWComparison
                     test/MonitoringComparison.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}   ByteStreamData TrigFTK_RawData TrigFTKSimLib TrigInDetAnalysisUtils TrigInDetAnalysis TrigFTK_RawData FTKStandaloneMonitoringLib 
		     )

atlas_install_headers( FTKStandaloneMonitoring )
