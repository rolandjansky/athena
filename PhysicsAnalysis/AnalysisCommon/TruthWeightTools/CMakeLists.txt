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
    set( extra_deps Control/AthToolSupport/AsgTools Control/AthAnalysisBaseComps)
    set( extra_include_dirs ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} )
    set( extra_link_libraries ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AthAnalysisBaseCompsLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTruth
                          ${extra_deps}
                          PRIVATE
                          Event/xAOD/xAODEventInfo
)

# External dependencies:
if( NOT XAOD_STANDALONE )
    find_package( HepMC )
    find_package( HepPDT )
endif()

# Build a shared library
atlas_add_library( TruthWeightToolsLib
                   TruthWeightTools/*.h Root/*.cxx
                   PUBLIC_HEADERS TruthWeightTools
                   INCLUDE_DIRS ${extra_include_dirs}
                   LINK_LIBRARIES AsgTools xAODTruth ${extra_link_libraries}
                   PRIVATE_LINK_LIBRARIES xAODEventInfo ${extra_private_link_libraries} )

# Component(s) in the package:
if( NOT XAOD_STANDALONE )
    atlas_add_component( TruthWeightTools
                         src/components/*.cxx
                         LINK_LIBRARIES TruthWeightToolsLib )
endif()
