################################################################################
# Package: ForwardRegion_SimEv
################################################################################

# Declare the package name:
atlas_subdir( ForwardRegion_SimEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          Simulation/HitManagement
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ForwardRegion_SimEv
                   src/*.cxx
                   PUBLIC_HEADERS ForwardRegion_SimEv
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} SGTools HitManagement
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( ForwardRegion_SimEvDict
                      ForwardRegion_SimEv/SimulationHitDict.h
                      ForwardRegion_SimEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} SGTools HitManagement GaudiKernel ForwardRegion_SimEv )

