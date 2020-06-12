################################################################################
# Package: L1TopoCoreSim
################################################################################

# Declare the package name:
atlas_subdir( L1TopoCoreSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigT1/L1Topo/L1TopoCommon
                          Trigger/TrigT1/L1Topo/L1TopoConfig
                          Trigger/TrigT1/L1Topo/L1TopoEvent
                          Trigger/TrigT1/L1Topo/L1TopoInterfaces
                          PRIVATE
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigT1/L1Topo/L1TopoAlgorithms
                          Trigger/TrigT1/L1Topo/L1TopoHardware )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree pthread RIO Hist MathCore )

# Component(s) in the package:
atlas_disable_as_needed()
atlas_add_library( L1TopoCoreSim
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoCoreSim
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES L1TopoCommon L1TopoConfig L1TopoEvent L1TopoInterfaces ${ROOT_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase L1TopoAlgorithms L1TopoHardware )

atlas_add_executable( TrigConfTopoStandAlone
                      src/test/TopoStandAlone.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} L1TopoCommon L1TopoConfig L1TopoEvent L1TopoInterfaces TrigConfBase L1TopoAlgorithms L1TopoHardware L1TopoCoreSim )

atlas_add_executable( TrigConfTopoTestSteeringConfig
                      src/test/TopoTestSteeringConfig.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} L1TopoCommon L1TopoConfig L1TopoEvent L1TopoInterfaces TrigConfBase L1TopoAlgorithms L1TopoHardware L1TopoCoreSim )

