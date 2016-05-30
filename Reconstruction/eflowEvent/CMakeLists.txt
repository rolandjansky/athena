################################################################################
# Package: eflowEvent
################################################################################

# Declare the package name:
atlas_subdir( eflowEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthContainers
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/Navigation
                          Control/SGTools
                          Event/EventKernel
                          Event/FourMom
                          Event/NavFourMom
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( eflowEvent
                   src/*.cxx
                   PUBLIC_HEADERS eflowEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloEvent AthContainers AthLinks Navigation SGTools EventKernel FourMom NavFourMom xAODCaloEvent xAODPFlow xAODTracking GaudiKernel muonEvent Particle VxVertex )
atlas_generate_cliddb( eflowEvent )

atlas_add_dictionary( eflowEventDict
                      eflowEvent/eflowEventDict.h
                      eflowEvent/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloEvent AthContainers AthLinks Navigation SGTools EventKernel FourMom NavFourMom xAODCaloEvent xAODPFlow xAODTracking GaudiKernel muonEvent Particle VxVertex eflowEvent
                      NAVIGABLES eflowObjectContainer )

