################################################################################
# Package: MSVertexToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MSVertexToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MSVertexReconstruction/MSVertexUtils )

# Install files from the package:
atlas_install_headers( MSVertexToolInterfaces )

