################################################################################
# Package: TruthWeightTools
################################################################################

# Declare the package name:
atlas_subdir( TruthWeightTools )

# Extra dependencies based on the build environment:
set( extra_deps )
set( extra_libs )
if( NOT XAOD_STANDALONE )
    set( extra_deps Control/AthToolSupport/AsgTools Control/AthenaBaseComps GaudiKernel)
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTruth
                          ${extra_deps}
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          Generators/TruthUtils )

# External dependencies:
if( NOT XAOD_STANDALONE )
    find_package( HepMC )
    find_package( HepPDT )
endif()

# Component(s) in the package:
if( NOT XAOD_STANDALONE )
    atlas_add_library( xAODTruthMetaDataTool
                       Root/*.cxx
                       PUBLIC_HEADERS TruthWeightTools
                       INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                       LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AsgTools AthenaBaseComps xAODTruth GaudiKernel
                       PRIVATE_LINK_LIBRARIES xAODEventInfo GeneratorObjects TruthUtils )
endif()

if( NOT XAOD_STANDALONE )
    atlas_add_component( TruthWeightTools
                         INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                         LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AsgTools AthenaBaseComps xAODTruth GaudiKernel xAODEventInfo GeneratorObjects TruthUtils xAODTruthMetaDataTool )
endif()
