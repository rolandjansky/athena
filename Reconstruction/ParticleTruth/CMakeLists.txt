################################################################################
# Package: ParticleTruth
################################################################################

# Declare the package name:
atlas_subdir( ParticleTruth )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Generators/GeneratorObjects
                          Reconstruction/Particle )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ParticleTruth
                   src/*.cxx
                   PUBLIC_HEADERS ParticleTruth
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools GeneratorObjects Particle
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( ParticleTruthDict
                      ParticleTruth/ParticleTruthDict.h
                      ParticleTruth/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GeneratorObjects Particle ParticleTruth
                      DATA_LINKS TrackParticleTruthCollection )

