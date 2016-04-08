################################################################################
# Package: SpecialUtils
################################################################################

# Declare the package name:
atlas_subdir( SpecialUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent )

# External dependencies:
find_package( CLHEP )

# Install files from the package:
atlas_install_headers( SpecialUtils )

