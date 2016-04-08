################################################################################
# Package: TrigNtupleBase
################################################################################

# Declare the package name:
atlas_subdir( TrigNtupleBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigNtupleBase
                   src/*.cxx
                   PUBLIC_HEADERS TrigNtupleBase
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests TrigDecisionToolLib )

