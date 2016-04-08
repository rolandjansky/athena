################################################################################
# Package: InDetDNASeparator
################################################################################

# Declare the package name:
atlas_subdir( InDetDNASeparator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          PRIVATE
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( InDetDNASeparator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkMaterialOnTrack TrkParameters TrkFitterInterfaces TrkFitterUtils EventPrimitives GaudiKernel InDetIdentifier TrkSurfaces TrkMeasurementBase TrkTrack )

# Install files from the package:
atlas_install_headers( InDetDNASeparator )

