################################################################################
# Package: JetEvent
################################################################################

# Declare the package name:
atlas_subdir( JetEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/Navigation
                          Control/SGTools
                          Control/StoreGate
                          Event/EventKernel
                          Event/FourMom
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetEvent
                   src/*.cxx
                   PUBLIC_HEADERS JetEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} DataModel Navigation SGTools EventKernel FourMom NavFourMom GaudiKernel ParticleEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( JetEventDict
                      JetEvent/JetEventDict.h
                      JetEvent/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModel Navigation SGTools StoreGateLib SGtests EventKernel FourMom NavFourMom GaudiKernel ParticleEvent JetEvent
                      NAVIGABLES JetCollection )

