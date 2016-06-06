################################################################################
# Package: VxMultiVertex
################################################################################

# Declare the package name:
atlas_subdir( VxMultiVertex )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Event/xAOD/xAODTracking
                          DetectorDescription/GeoPrimitives )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( VxMultiVertex
                   src/*.cxx
                   PUBLIC_HEADERS VxMultiVertex
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools TrkParameters TrkParticleBase TrkTrack VxVertex xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( VxMultiVertexDict
                      VxMultiVertex/VxMultiVertexDict.h
                      VxMultiVertex/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools TrkParameters TrkParticleBase TrkTrack VxVertex VxMultiVertex
                      ELEMENT_LINKS MVFVxContainer )

