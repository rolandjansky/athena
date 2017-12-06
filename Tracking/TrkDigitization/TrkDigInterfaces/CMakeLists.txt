################################################################################
# Package: TrkDigInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkDigInterfaces )
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkDigitization/TrkDigEvent )

# Component(s) in the package:
atlas_add_library( TrkDigInterfaces
                     PUBLIC_HEADERS TrkDigInterfaces
                     LINK_LIBRARIES GaudiKernel TrkDigEvent )
# Install files from the package:
atlas_install_headers( TrkDigInterfaces )

