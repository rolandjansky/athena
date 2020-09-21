################################################################################
# Package: CaloClusterMatching
################################################################################

# Declare the package name:
atlas_subdir( CaloClusterMatching )

# Component(s) in the package:
atlas_add_component( CaloClusterMatching
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODCaloEvent GaudiKernel AthenaBaseComps xAODCore )

# Install files from the package:
atlas_install_headers( CaloClusterMatching )

