################################################################################
# Package: iPatQuality
################################################################################

# Declare the package name:
atlas_subdir( iPatQuality )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Reconstruction/iPat/iPatInterfaces
                          Reconstruction/iPat/iPatTrack
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( iPatQuality
                     src/TrackQuality.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps iPatInterfaces iPatTrack GaudiKernel )

# Install files from the package:
atlas_install_headers( iPatQuality )

