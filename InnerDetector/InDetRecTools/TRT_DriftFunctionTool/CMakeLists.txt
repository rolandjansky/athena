# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_DriftFunctionTool )

# External dependencies:
find_package( CLHEP )

atlas_add_library( TRT_DriftFunctionToolLib
                   TRT_DriftFunctionTool/*..h
                   INTERFACE
                   PUBLIC_HEADERS TRT_DriftFunctionTool
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( TRT_DriftFunctionTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry GeoModelInterfaces GeoModelUtilities TRT_ConditionsServicesLib InDetIdentifier TRT_DriftFunctionToolLib )
