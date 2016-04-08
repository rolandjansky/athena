################################################################################
# Package: McParticleAlgs
################################################################################

# Declare the package name:
atlas_subdir( McParticleAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/NavFourMom
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleKernel
                          PhysicsAnalysis/TruthParticleID/McParticleUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( McParticleAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps StoreGateLib SGtests NavFourMom McParticleEvent McParticleKernel McParticleUtils )

# Install files from the package:
atlas_install_headers( McParticleAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

