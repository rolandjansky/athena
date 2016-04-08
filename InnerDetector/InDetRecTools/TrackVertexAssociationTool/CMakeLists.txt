################################################################################
# Package: TrackVertexAssociationTool
################################################################################

# Declare the package name:
atlas_subdir( TrackVertexAssociationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthToolSupport/AsgTools
                          Control/SGTools
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrackVertexAssociationToolLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS TrackVertexAssociationTool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AsgTools SGTools xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps )

atlas_add_component( TrackVertexAssociationTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AsgTools SGTools xAODTracking GaudiKernel AthenaBaseComps TrackVertexAssociationToolLib )

atlas_add_dictionary( TrackVertexAssociationToolDict
                      TrackVertexAssociationTool/TrackVertexAssociationToolDict.h
                      TrackVertexAssociationTool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AsgTools SGTools xAODTracking GaudiKernel AthenaBaseComps TrackVertexAssociationToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

