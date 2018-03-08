################################################################################
# Package: TrigCaloDataAccessSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloDataAccessSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel

                          PRIVATE
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo
                          Event/xAOD/xAODTrigCalo
                          GaudiKernel
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamData
			              AtlasTest/TestTools
			              Control/AthenaMonitoring )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigCaloDataAccessSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigCaloDataAccessSvc
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrigSteeringEvent ByteStreamCnvSvcBaseLib TrigInterfacesLib ByteStreamData_test 
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities ByteStreamData TrigT1Interfaces )

atlas_add_component( TrigCaloDataAccessSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES TrigCaloDataAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( TestService 
		SCRIPT test/test_dataaccess.sh
		PROPERTIES TIMEOUT 1200)
