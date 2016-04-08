################################################################################
# Package: RecAlgs
################################################################################

# Declare the package name:
atlas_subdir( RecAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Reconstruction/RecEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( RecAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils StoreGateLib SGtests xAODEventInfo GaudiKernel RecEvent )

