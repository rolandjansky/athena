################################################################################
# Package: CaloClusterMatching
################################################################################

# Declare the package name:
atlas_subdir( CaloClusterMatching )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Event/xAOD/xAODCore )

# Component(s) in the package:
atlas_add_component( CaloClusterMatching
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODCaloEvent GaudiKernel AthenaBaseComps xAODCore )

# Install files from the package:
atlas_install_headers( CaloClusterMatching )

