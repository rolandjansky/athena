# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetVxInJetTool )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigInDetVxInJetTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AnalysisUtilsLib AthenaBaseComps GaudiKernel GeoPrimitives TrigInDetEvent TrigInDetToolInterfacesLib TrigInterfacesLib TrigVKalFitterLib VxVertex )
