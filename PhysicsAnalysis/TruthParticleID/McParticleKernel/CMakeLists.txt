################################################################################
# Package: McParticleKernel
################################################################################

# Declare the package name:
atlas_subdir( McParticleKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/AtlasHepMC
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( McParticleKernel
                   src/*.cxx
                   PUBLIC_HEADERS McParticleKernel
                   LINK_LIBRARIES GaudiKernel AtlasHepMCLib )

