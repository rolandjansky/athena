################################################################################
# Package: FlowAfterburner
################################################################################

# Declare the package name:
atlas_subdir( FlowAfterburner )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          PRIVATE
                          Control/StoreGate
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FlowAfterburnerLib
                   src/*.cxx
                   PUBLIC_HEADERS FlowAfterburner
                   INCLUDE_DIRS ${GSL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GSL_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TruthHelper GeneratorObjects StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AtlasHepMCLib AtlasHepMCsearchLib TruthUtils )

atlas_add_component( FlowAfterburner
                     src/components/*.cxx
                     INCLUDE_DIRS ${GSL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}  ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GSL_LIBRARIES} ${ROOT_LIBRARIES} AtlasHepMCLib  AtlasHepMCsearchLib ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TruthHelper GeneratorObjects StoreGateLib SGtests TruthUtils FlowAfterburnerLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

