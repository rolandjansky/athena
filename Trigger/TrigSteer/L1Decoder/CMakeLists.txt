################################################################################
# Package: L1Decoder
################################################################################

# Declare the package name:
atlas_subdir( L1Decoder )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          GaudiKernel
                          PRIVATE
                          Control/AthViews
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/AthenaMonitoring
                          Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result
			              Trigger/TrigSteer/DecisionHandling )

find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( L1DecoderLib
                   src/*.cxx
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PUBLIC_HEADERS L1Decoder
                   LINK_LIBRARIES GaudiKernel AthViews AthenaBaseComps AthenaMonitoringLib 
                   CxxUtils xAODTrigger TrigConfHLTData TrigConfL1Data
		           TrigSteeringEvent TrigT1Interfaces TrigT1Result DecisionHandlingLib
                   LINK_LIBRARIES ${CLHEP_LIBRARIES}
                   )
                   
atlas_add_component( L1Decoder
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES L1DecoderLib ${CLHEP_LIBRARIES}
                     )

atlas_install_python_modules( python/*.py)
# Install files from the package:
atlas_install_headers( L1Decoder )
atlas_install_joboptions( share/*.py )
