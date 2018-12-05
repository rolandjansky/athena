################################################################################
# Package: TrigDecisionEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigDecisionEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigDecisionEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigDecisionEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks AthenaKernel TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( TrigDecisionEventDict
                      TrigDecisionEvent/TrigDecisionEventDict.h
                      TrigDecisionEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthenaKernel TrigSteeringEvent TestTools TrigDecisionEvent
                      DATA_LINKS HLT::HLTResult )

