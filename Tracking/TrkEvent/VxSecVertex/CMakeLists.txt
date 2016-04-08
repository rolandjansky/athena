################################################################################
# Package: VxSecVertex
################################################################################

# Declare the package name:
atlas_subdir( VxSecVertex )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxJetVertex
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( VxSecVertex
                   src/*.cxx
                   PUBLIC_HEADERS VxSecVertex
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools xAODBase xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrkParticleBase VxJetVertex VxVertex )

atlas_add_dictionary( VxSecVertexDict
                      VxSecVertex/VxSecVertexDict.h
                      VxSecVertex/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools xAODBase xAODTracking TrkParticleBase VxJetVertex VxVertex VxSecVertex )

