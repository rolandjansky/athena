#####################################
# Flavor tagging discriminant tools
#####################################
#
# This package is a collection of 'duel-use' tools to calculate
# high-level flavor tagging discriminants. Because it should work both
# inside and outside Athena, nothing here can use the magnetic field,
# atlas geometry, or material maps, but neural networks etc are all
# fine.


# Declare the package name:
atlas_subdir( FlavorTagDiscriminants )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODBase
  Event/xAOD/xAODJet
  Event/xAOD/xAODEventInfo)

# Build a shared library:
atlas_add_library( FlavorTagDiscriminants
  Root/BTagJetAugmenter.cxx
  PUBLIC_HEADERS FlavorTagDiscriminants
  LINK_LIBRARIES xAODBase xAODJet xAODEventInfo)
