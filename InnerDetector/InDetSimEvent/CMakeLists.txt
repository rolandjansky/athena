################################################################################
# Package: InDetSimEvent
################################################################################

# Declare the package name:
atlas_subdir( InDetSimEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthAllocators
                          Control/CxxUtils
                          Generators/GeneratorObjects
                          Simulation/HitManagement
                          
                          PRIVATE
                          Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS InDetSimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaKernel CxxUtils GeneratorObjects HitManagement StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} InDetIdentifier )

atlas_add_dictionary( InDetSimEventDict
                      InDetSimEvent/InDetSimEventDict.h
                      InDetSimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthAllocators CxxUtils GeneratorObjects HitManagement StoreGateLib SGtests InDetIdentifier InDetSimEvent )

