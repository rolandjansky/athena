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

# we use lwtnn in here a bit
find_package( lwtnn )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Reconstruction/Jet/JetInterface
  Event/xAOD/xAODBase
  Event/xAOD/xAODJet
  Event/xAOD/xAODEventInfo
  PRIVATE
  Tools/PathResolver
  )

# Build a shared library:
atlas_add_library( FlavorTagDiscriminants
  Root/BTagJetAugmenter.cxx
  Root/BTagTrackAugmenter.cxx
  Root/DL2.cxx
  Root/DL2HighLevel.cxx
  Root/DL2Tool.cxx
  Root/customGetter.cxx
  INCLUDE_DIRS ${LWTNN_INCLUDE_DIRS}
  PUBLIC_HEADERS FlavorTagDiscriminants
  LINK_LIBRARIES AsgTools xAODBase xAODJet xAODEventInfo PathResolver
  ${LWTNN_LIBRARIES})

if (NOT XAOD_STANDALONE)
  atlas_add_component( FlavorTagDiscriminantsLib
    src/components/FlavorTagDiscriminants_entries.cxx
    src/components/FlavorTagDiscriminants_load.cxx
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} FlavorTagDiscriminants
    )
endif()

atlas_add_dictionary( FlavorTagDiscriminantsDict
   FlavorTagDiscriminants/FlavorTagDiscriminantsDict.h
   FlavorTagDiscriminants/selection.xml
   LINK_LIBRARIES FlavorTagDiscriminants )


atlas_install_python_modules( python/*.py )
