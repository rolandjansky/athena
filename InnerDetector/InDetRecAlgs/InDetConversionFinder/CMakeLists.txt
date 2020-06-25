################################################################################
# Package: InDetConversionFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetConversionFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Reconstruction/egamma/egammaInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary )

# Component(s) in the package:
atlas_add_component( InDetConversionFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces TrkTrack TrkTrackSummary egammaInterfacesLib )

# Install files from the package:
atlas_install_headers( InDetConversionFinder )

