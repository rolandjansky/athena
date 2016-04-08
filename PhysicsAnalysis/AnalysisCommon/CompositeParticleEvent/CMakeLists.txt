################################################################################
# Package: CompositeParticleEvent
################################################################################

# Declare the package name:
atlas_subdir( CompositeParticleEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent )

# Install files from the package:
atlas_install_headers( CompositeParticleEvent )

