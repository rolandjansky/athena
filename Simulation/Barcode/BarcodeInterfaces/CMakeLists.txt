################################################################################
# Package: BarcodeInterfaces
################################################################################

# Declare the package name:
atlas_subdir( BarcodeInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/Barcode/BarcodeEvent )

# Install files from the package:
atlas_install_headers( BarcodeInterfaces )

