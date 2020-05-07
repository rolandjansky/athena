################################################################################
# Package: BarcodeInterfaces
################################################################################

# Declare the package name:
atlas_subdir( BarcodeInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/Barcode/BarcodeEvent )

atlas_add_library( BarcodeInterfacesLib
                   BarcodeInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS BarcodeInterfaces
                   LINK_LIBRARIES BarcodeEventLib GaudiKernel )

