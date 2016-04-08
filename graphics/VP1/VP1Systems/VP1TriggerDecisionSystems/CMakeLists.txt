################################################################################
# Package: VP1TriggerDecisionSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1TriggerDecisionSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigDecision
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigSteering )

# Component(s) in the package:
atlas_add_library( VP1TriggerDecisionSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1TriggerDecisionSystems
                   LINK_LIBRARIES VP1Base TrigDecisionToolLib TrigSteeringLib
                   PRIVATE_LINK_LIBRARIES TrigSteeringEvent )

