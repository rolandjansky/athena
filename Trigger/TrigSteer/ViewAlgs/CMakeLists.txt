################################################################################
# Package: ViewAlgs
################################################################################

# Declare the package name:
atlas_subdir( ViewAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigger
                          GaudiKernel
						  Control/AthContainers
						  Control/AthLinks 
                          PRIVATE
                          Control/AthViews
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/L1Decoder )

# Component(s) in the package:
atlas_add_library( ViewAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS ViewAlgs
                   LINK_LIBRARIES xAODTrigger GaudiKernel AthViews
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils TrigConfHLTData TrigSteeringEvent )

atlas_add_component( ViewAlgs
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigger GaudiKernel AthViews AthenaBaseComps CxxUtils TrigConfHLTData TrigSteeringEvent ViewAlgsLib )

