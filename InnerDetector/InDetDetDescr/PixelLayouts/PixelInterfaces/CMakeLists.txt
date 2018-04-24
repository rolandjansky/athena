################################################################################
# Package: PixelInterfaces
################################################################################

# Declare the package name:
atlas_subdir( PixelInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  Control/AthenaKernel
			  GaudiKernel )

# Component(s) in the package:
atlas_install_headers( PixelInterfaces )

