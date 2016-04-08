################################################################################
# Package: TrigT1Result
################################################################################

# Declare the package name:
atlas_subdir( TrigT1Result )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel
                          Trigger/TrigT1/L1Topo/L1TopoRDO
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          Control/AthenaKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_library( TrigT1Result
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1Result
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} SGTools GaudiKernel L1TopoRDO TrigT1Interfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel )

atlas_add_dictionary( TrigT1ResultDict
                      TrigT1Result/TrigT1ResultDict.h
                      TrigT1Result/selection.xml
                      INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${ROOT_LIBRARIES} SGTools GaudiKernel L1TopoRDO TrigT1Interfaces AthenaKernel TrigT1Result )

