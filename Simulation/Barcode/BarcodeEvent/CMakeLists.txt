################################################################################
# Package: BarcodeEvent
################################################################################

# Declare the package name:
atlas_subdir( BarcodeEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

atlas_add_library( BarcodeEventLib
                   BarcodeEvent/*.h
                   INTERFACE
                   PUBLIC_HEADERS BarcodeEvent )
