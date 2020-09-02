################################################################################
# Package: InDetTruthInterfaces
################################################################################

# Declare the package name:
atlas_subdir( InDetTruthInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

atlas_add_library( InDetTruthInterfaces
                   InDetTruthInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetTruthInterfaces
                   LINK_LIBRARIES GaudiKernel )
