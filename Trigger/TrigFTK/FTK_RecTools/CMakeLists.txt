################################################################################
# Package: FTK_RecTools
################################################################################

# Declare the package name:
atlas_subdir( FTK_RecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tools/PyJobTransforms
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigFTK/FTK_RecToolInterfaces
                          Trigger/TrigFTK/TrigFTK_RawData
                          Event/xAOD/xAODTracking
                          Tracking/TrkVertexFitter/TrkVxEdmCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FTK_RecToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS FTK_RecTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrkTrack VxVertex FTK_DataProviderInterfaces TrigFTK_RawData xAODTracking TrkVxEdmCnvLib)

atlas_add_component( FTK_RecTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrkTrack VxVertex FTK_DataProviderInterfaces TrigFTK_RawData xAODTracking TrkVxEdmCnvLib FTK_RecToolsLib )

