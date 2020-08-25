################################################################################
# Package: MSVertexToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MSVertexToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MSVertexReconstruction/MSVertexUtils )

atlas_add_library( MSVertexToolInterfaces
                   MSVertexToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MSVertexToolInterfaces
                   LINK_LIBRARIES GaudiKernel MSVertexUtils )


