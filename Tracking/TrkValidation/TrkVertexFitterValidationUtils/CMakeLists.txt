################################################################################
# Package: TrkVertexFitterValidationUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexFitterValidationUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Generators/GeneratorObjects
                          Reconstruction/ParticleTruth
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( TrkVertexFitterValidationUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexFitterValidationUtils
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} GeneratorObjects ParticleTruth TrkParticleBase VxVertex )

atlas_add_component( TrkVertexFitterValidationUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel GeneratorObjects ParticleTruth TrkParticleBase VxVertex TrkVertexFitterValidationUtilsLib )

