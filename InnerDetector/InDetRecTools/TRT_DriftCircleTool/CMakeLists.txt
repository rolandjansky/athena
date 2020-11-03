# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_DriftCircleTool )

atlas_add_library( TRT_DriftCircleToolLib
                   TRT_DriftCircleTool/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_DriftCircleTool
                   LINK_LIBRARIES GaudiKernel InDetPrepRawData InDetRawData TrkPrepRawData AthenaBaseComps TRT_ConditionsServicesLib TRT_ReadoutGeometry CommissionEvent StoreGateLib LumiBlockData )

# Component(s) in the package:
atlas_add_component( TRT_DriftCircleTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES EventPrimitives GaudiKernel GeoPrimitives InDetConditionsSummaryService InDetIdentifier InDetPrepRawData InDetRawData StoreGateLib TRT_ConditionsServicesLib TRT_DriftCircleToolLib TRT_DriftFunctionToolLib TRT_ReadoutGeometry xAODEventInfo )
