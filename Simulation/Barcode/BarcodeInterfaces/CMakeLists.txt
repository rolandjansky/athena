################################################################################
# Package: BarcodeInterfaces
################################################################################

# Declare the package name:
atlas_subdir( BarcodeInterfaces )

atlas_add_library( BarcodeInterfacesLib
                   BarcodeInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS BarcodeInterfaces
                   LINK_LIBRARIES BarcodeEventLib GaudiKernel )

