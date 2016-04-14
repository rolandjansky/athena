################################################################################
# Package: TruthHelper
################################################################################

# Declare the package name:
atlas_subdir( TruthHelper )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorObjects
                          PRIVATE
                          Control/AthenaKernel
                          Generators/TruthUtils )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TruthHelper
                   src/*.cxx
                   PUBLIC_HEADERS TruthHelper
                   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${HEPMC_LIBRARIES} GaudiKernel GeneratorObjects StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel TruthUtils )

atlas_add_dictionary( TruthHelperDict
                      TruthHelper/TruthHelperDict.h
                      TruthHelper/selection.xml
                      INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} StoreGateLib SGtests GaudiKernel GeneratorObjects AthenaKernel TruthUtils TruthHelper )

