################################################################################
# Package: JetTagEvent
################################################################################

# Declare the package name:
atlas_subdir( JetTagEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          Control/Navigation
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle
                          PRIVATE
                          Event/EventKernel
                          Event/NavFourMom
                          Tracking/TrkEvent/VxSecVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetTagEvent
                   src/*.cxx
                   PUBLIC_HEADERS JetTagEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloGeoHelpers Navigation ParticleEvent JetEvent Particle
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventKernel NavFourMom VxSecVertex )

atlas_add_dictionary( JetTagEventDict
                      JetTagEvent/JetTagEventDict.h
                      JetTagEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers Navigation ParticleEvent JetEvent Particle EventKernel NavFourMom VxSecVertex JetTagEvent
                      NAVIGABLES  )

