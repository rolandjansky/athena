################################################################################
# Package: tauEvent
################################################################################

# Declare the package name:
atlas_subdir( tauEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/DataModel
                          Control/Navigation
                          Control/SGTools
                          Event/EventKernel
                          Event/FourMom
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# this line failed automatic conversion in cmt2cmake :
# macro_append tauEventDict_shlibflags

# Component(s) in the package:
atlas_add_library( tauEvent
                   src/*.cxx
                   PUBLIC_HEADERS tauEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier DataModel Navigation SGTools EventKernel FourMom NavFourMom GaudiKernel ParticleEvent JetEvent Particle VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( tauEventDict
                      tauEvent/tauEventDict.h
                      tauEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier DataModel Navigation SGTools EventKernel FourMom NavFourMom GaudiKernel ParticleEvent JetEvent Particle VxVertex tauEvent
                      ELEMENT_LINKS Analysis::TauJetContainer  Analysis::TauDetailsContainer )

atlas_add_dictionary( tauEnumsDict
                      tauEvent/tauEnumsDict.h
                      tauEvent/selectionEnums.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier DataModel Navigation SGTools EventKernel FourMom NavFourMom GaudiKernel ParticleEvent JetEvent Particle VxVertex tauEvent )

