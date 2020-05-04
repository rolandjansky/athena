################################################################################
# Package: MissingETEvent
################################################################################

# Declare the package name:
atlas_subdir( MissingETEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/Navigation
                          Event/EventKernel
                          Event/NavFourMom
                          GaudiKernel
                          PRIVATE
                          Generators/AtlasHepMC
                          Control/CxxUtils
                          Generators/GenAnalysisTools/TruthHelper )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MissingETEvent
                   src/*.cxx
                   PUBLIC_HEADERS MissingETEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AtlasHepMCLib CaloEvent CaloGeoHelpers AthContainers Navigation EventKernel NavFourMom GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils TruthHelper )

atlas_add_dictionary( MissingETEventDict
                      MissingETEvent/MissingETEventDict.h
                      MissingETEvent/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib CaloEvent CaloGeoHelpers AthContainers Navigation EventKernel NavFourMom GaudiKernel CxxUtils TruthHelper MissingETEvent )

