################################################################################
# Package: MSVertexToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MSVertexToolInterfaces )

atlas_add_library( MSVertexToolInterfaces
                   MSVertexToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MSVertexToolInterfaces
                   LINK_LIBRARIES GaudiKernel MSVertexUtils )


