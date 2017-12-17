atlas_subdir(FTKStandaloneMonitoring)

atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamData
                          Trigger/TrigFTK/TrigFTK_RawData
			  Trigger/TrigAnalysis/TrigInDetAnalysis
			  Trigger/TrigAnalysis/TrigInDetAnalysisUtils
                          PRIVATE
                          Trigger/TrigFTK/TrigFTKSim
                          )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write ) 
find_package( tdaq COMPONENTS rc_RunController ipc omnithread omniORB4 rc RunControl rc_OnlSvc ohroot oh cmdline is owl)

atlas_add_library(FTKStandaloneMonitoring
                  src/*.cxx
                  PUBLIC_HEADERS FTKStandaloneMonitoring
                  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                  LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES}  ByteStreamData   TrigFTK_RawData TrigFTKSimLib TrigInDetAnalysisUtils TrigInDetAnalysis TrigFTK_RawData
                  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_executable(MonitoringFTKHWSWComparison
                     test/MonitoringComparison.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}  ${TDAQ_LIBRARIES} ByteStreamData TrigFTK_RawData TrigFTKSimLib TrigInDetAnalysisUtils TrigInDetAnalysis TrigFTK_RawData FTKStandaloneMonitoring 
		     )

atlas_install_headers( FTKStandaloneMonitoring )
