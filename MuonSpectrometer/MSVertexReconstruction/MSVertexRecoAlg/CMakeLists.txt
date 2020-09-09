################################################################################
# Package: MSVertexRecoAlg
################################################################################

# Declare the package name:
atlas_subdir( MSVertexRecoAlg )

# Component(s) in the package:
atlas_add_component( MSVertexRecoAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MSVertexUtils MSVertexToolInterfaces )

# Install files from the package:
atlas_install_headers( MSVertexRecoAlg )

