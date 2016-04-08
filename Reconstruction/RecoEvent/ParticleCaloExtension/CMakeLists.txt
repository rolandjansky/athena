################################################################################
# Package: ParticleCaloExtension
################################################################################

# Declare the package name:
atlas_subdir( ParticleCaloExtension )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthContainers
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore )

# Component(s) in the package:
atlas_add_library( ParticleCaloExtension
                   src/*.cxx
                   PUBLIC_HEADERS ParticleCaloExtension
                   LINK_LIBRARIES CaloEvent AthContainers xAODCaloEvent xAODCore )

