################################################################################
# Package: McAsciiEventSelector
################################################################################

# Declare the package name:
atlas_subdir( McAsciiEventSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventInfo
                          PhysicsAnalysis/TruthParticleID/McParticleKernel )

# Component(s) in the package:
atlas_add_component( McAsciiEventSelector
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaKernel EventInfo McParticleKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

