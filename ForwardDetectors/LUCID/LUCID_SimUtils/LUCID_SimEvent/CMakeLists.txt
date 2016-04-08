################################################################################
# Package: LUCID_SimEvent
################################################################################

# Declare the package name:
atlas_subdir( LUCID_SimEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects
                          Simulation/HitManagement )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LUCID_SimEvent
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_SimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel SGTools Identifier GeneratorObjects HitManagement
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( LUCID_SimEventDict
                      LUCID_SimEvent/LUCID_SimEventDict.h
                      LUCID_SimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel SGTools Identifier GeneratorObjects HitManagement LUCID_SimEvent )

