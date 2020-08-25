################################################################################
# Package: TrigOnlineMonitor
################################################################################

# Declare the package name:
atlas_subdir( TrigOnlineMonitor )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( TrigOnlineMonitor
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps ByteStreamData ByteStreamData_test GaudiKernel AthenaInterprocess AthenaKernel ByteStreamCnvSvcBaseLib EventInfo MagFieldInterfaces TrigConfL1Data TrigSteeringEvent TrigMonitorBaseLib TrigSteeringLib L1TopoConfig L1TopoRDO TrigT1Interfaces TrigT1Result TrigT1ResultByteStreamLib TrigConfData)

# Install files from the package:
atlas_install_headers( TrigOnlineMonitor )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/plotL1TopoROBMonHist.py )

