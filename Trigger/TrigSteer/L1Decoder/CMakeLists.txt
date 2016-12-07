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
			  # not needed now                          Control/AthenaMonitoring
                          Control/CxxUtils
                          Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result
			  Trigger/TrigMonitoring/TrigMonitorBase )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( L1DecoderLib
                   src/*.cxx
                   PUBLIC_HEADERS L1Decoder
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel AthViews AthenaBaseComps  CxxUtils xAODTrigger TrigConfHLTData TrigConfL1Data TrigSteeringEvent TrigT1Interfaces TrigT1Result  TrigMonitorBaseLib
                   )
                   
atlas_add_component( L1Decoder
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel AthViews AthenaBaseComps CxxUtils xAODTrigger TrigConfHLTData TrigConfL1Data TrigSteeringEvent TrigT1Interfaces TrigT1Result L1DecoderLib 
                     )

# Install files from the package:
atlas_install_headers( L1Decoder )
atlas_install_joboptions( share/*.py )
