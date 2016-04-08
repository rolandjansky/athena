################################################################################
# Package: HSG2Event
################################################################################

# Declare the package name:
atlas_subdir( HSG2Event )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( HSG2Event
                   src/*.cxx
                   PUBLIC_HEADERS HSG2Event
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools ParticleEvent VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( HSG2EventDict
                      HSG2Event/HSG2EventDict.h
                      HSG2Event/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools ParticleEvent VxVertex HSG2Event )

