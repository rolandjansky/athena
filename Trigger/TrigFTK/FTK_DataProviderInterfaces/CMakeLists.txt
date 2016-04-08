################################################################################
# Package: FTK_DataProviderInterfaces
################################################################################

# Declare the package name:
atlas_subdir( FTK_DataProviderInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/IRegionSelector
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tools/PyJobTransforms
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigFTK/TrigFTK_RawData )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FTK_DataProviderInterfaces
                   PUBLIC_HEADERS FTK_DataProviderInterfaces
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel IRegionSelector xAODTracking GaudiKernel TrkTrack TrigFTK_RawData )

