################################################################################
# Package: FTK_RecToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( FTK_RecToolInterfaces )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tools/PyJobTransforms
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigFTK/TrigFTK_RawData
                          Event/xAOD/xAODTracking  )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_headers( FTK_RecToolInterfaces )

