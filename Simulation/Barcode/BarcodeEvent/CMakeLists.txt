################################################################################
# Package: BarcodeEvent
################################################################################

# Declare the package name:
atlas_subdir( BarcodeEvent )

atlas_add_library( BarcodeEventLib
                   BarcodeEvent/*.h
                   INTERFACE
                   PUBLIC_HEADERS BarcodeEvent )
