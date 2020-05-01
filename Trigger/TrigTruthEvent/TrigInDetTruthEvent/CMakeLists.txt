################################################################################
# Package: TrigInDetTruthEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetTruthEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaKernel
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigInDetTruthEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetTruthEvent
                   INCLUDE_DIRS 
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AtlasHepMCLib AthContainers AthLinks AthenaKernel GeneratorObjects TrigInDetEvent TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( TrigInDetTruthEventDict
                      TrigInDetTruthEvent/TrigInDetTruthEventDict.h
                      TrigInDetTruthEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthContainers AthLinks GeneratorObjects TrigInDetEvent TrigSteeringEvent AthenaKernel GaudiKernel TrigInDetTruthEvent )

