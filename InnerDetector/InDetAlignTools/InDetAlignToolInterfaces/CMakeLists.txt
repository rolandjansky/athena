################################################################################
# Package: InDetAlignToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          GaudiKernel )

atlas_add_library( InDetAlignToolInterfaces
                   InDetAlignToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetAlignToolInterfaces
                   LINK_LIBRARIES GaudiKernel AthContainers )

