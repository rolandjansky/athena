################################################################################
# Package: TrigInDetTruthEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetTruthEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/SGTools
                          Generators/GeneratorObjects
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigInDetTruthEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetTruthEvent
                   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${HEPMC_LIBRARIES} AthContainers AthLinks SGTools GeneratorObjects TrigInDetEvent TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigInDetTruthEventDict
                      TrigInDetTruthEvent/TrigInDetTruthEventDict.h
                      TrigInDetTruthEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AthContainers AthLinks SGTools GeneratorObjects TrigInDetEvent TrigSteeringEvent AthenaKernel GaudiKernel TrigInDetTruthEvent )

