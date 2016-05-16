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
                          Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( L1Decoder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel AthViewsLib AthenaBaseComps CxxUtils xAODTrigger TrigConfHLTData TrigConfL1Data TrigSteeringEvent TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_headers( L1Decoder )
atlas_install_joboptions( share/*.py )
