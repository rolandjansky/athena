################################################################################
# Package: TrigCostD3PD
################################################################################

# Declare the package name:
atlas_subdir( TrigCostD3PD )

# External dependencies:
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_root_dictionary( TrigCostD3PD
                           TrigCostD3PDDictSource
                           ROOT_HEADERS TrigCostD3PD/*.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( TrigCostD3PD
                   Root/*.cxx
                   ${TrigCostD3PDDictSource}
                   PUBLIC_HEADERS TrigCostD3PD
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

