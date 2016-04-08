################################################################################
# Package: InDetResidualPullCalculators
################################################################################

# Declare the package name:
atlas_subdir( InDetResidualPullCalculators )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( InDetResidualPullCalculators
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRIO_OnTrack TrkEventPrimitives TrkParameters TrkToolInterfaces )

