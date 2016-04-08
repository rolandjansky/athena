################################################################################
# Package: PanTauEvent
################################################################################

# Declare the package name:
atlas_subdir( PanTauEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/CLIDSvc
                          Control/SGTools
                          Event/FourMom
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTau
                          Reconstruction/tauEvent
                          PRIVATE
                          GaudiKernel
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PanTauEvent
                   src/*.cxx
                   PUBLIC_HEADERS PanTauEvent
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers SGTools FourMom xAODPFlow xAODTau tauEvent
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel JetEvent Particle )

atlas_add_dictionary( PanTauEventDict
                      PanTauEvent/PanTauEventDict.h
                      PanTauEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers SGTools FourMom xAODPFlow xAODTau tauEvent GaudiKernel JetEvent Particle PanTauEvent )

