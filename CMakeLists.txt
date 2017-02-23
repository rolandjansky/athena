################################################################################
# Package: TruthWeightTools
################################################################################

# Declare the package name:
atlas_subdir( TruthWeightTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          Generators/TruthUtils )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_library( xAODTruthMetaDataTool
                   Root/*.cxx
                   PUBLIC_HEADERS TruthWeightTools
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AsgTools AthenaBaseComps xAODTruth GaudiKernel
                   PRIVATE_LINK_LIBRARIES xAODEventInfo GeneratorObjects TruthUtils )

atlas_add_component( TruthWeightTools
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AsgTools AthenaBaseComps xAODTruth GaudiKernel xAODEventInfo GeneratorObjects TruthUtils xAODTruthMetaDataTool )

