################################################################################
# Package: AthenaPoolKernel
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( AthenaPoolKernelLib
   AthenaPoolKernel/*.h
   INTERFACE
   PUBLIC_HEADERS AthenaPoolKernel
   LINK_LIBRARIES GaudiKernel )
