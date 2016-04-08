################################################################################
# Package: MuGirlSelection
################################################################################

# Declare the package name:
atlas_subdir( MuGirlSelection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuGirlInterfaces
                          Trigger/TrigEvent/TrigMuonEvent
                          PRIVATE
                          Reconstruction/Particle )

# Component(s) in the package:
atlas_add_component( MuGirlSelection
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuGirlInterfaces TrigMuonEvent Particle )

# Install files from the package:
atlas_install_headers( MuGirlSelection )

