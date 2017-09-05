################################################################################
# Package: TruthWeightTools
################################################################################

# Declare the package name:
atlas_subdir( TruthWeightTools )

# Extra dependencies based on the build environment:
set( extra_deps )
set( extra_libs )
set( extra_include_dirs )
set( extra_link_libraries )
set( extra_private_link_libraries )
if( NOT XAOD_STANDALONE )
    set( extra_deps Control/AthToolSupport/AsgTools Control/AthenaBaseComps GaudiKernel)
    set( extra_include_dirs ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} )
    set( extra_link_libraries ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel )
    set( extra_private_link_libraries GeneratorObjects )
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
atlas_add_library( xAODTruthMetaDataTool
                   TruthWeightTools/*.h Root/*.cxx
                   PUBLIC_HEADERS TruthWeightTools
                   INCLUDE_DIRS ${extra_include_dirs}
                   LINK_LIBRARIES AsgTools xAODTruth ${extra_link_libraries}
                   PRIVATE_LINK_LIBRARIES xAODEventInfo TruthUtils ${extra_private_link_libraries} )

if( NOT XAOD_STANDALONE )
    atlas_add_component( TruthWeightTools
                         INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                         LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AsgTools AthenaBaseComps xAODTruth GaudiKernel xAODEventInfo GeneratorObjects TruthUtils xAODTruthMetaDataTool )
endif()
