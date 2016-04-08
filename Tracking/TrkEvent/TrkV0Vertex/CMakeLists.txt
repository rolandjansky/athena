################################################################################
# Package: TrkV0Vertex
################################################################################

# Declare the package name:
atlas_subdir( TrkV0Vertex )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Event/EventKernel
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkV0Vertex
                   src/*.cxx
                   PUBLIC_HEADERS TrkV0Vertex
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools EventKernel VxVertex )

atlas_add_dictionary( TrkV0VertexDict
                      TrkV0Vertex/V0VertexDict.h
                      TrkV0Vertex/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools EventKernel VxVertex TrkV0Vertex
                      ELEMENT_LINKS V0Container )

