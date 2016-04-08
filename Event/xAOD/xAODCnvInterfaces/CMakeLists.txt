################################################################################
# Package: xAODCnvInterfaces
################################################################################

# Declare the package name:
atlas_subdir( xAODCnvInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( xAODCnvInterfaces
                   PUBLIC_HEADERS xAODCnvInterfaces
                   LINK_LIBRARIES GaudiKernel )

