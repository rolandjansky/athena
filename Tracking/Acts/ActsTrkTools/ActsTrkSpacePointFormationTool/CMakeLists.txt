# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkSpacePointFormationTool )

# External dependencies:

atlas_add_component( ActsTrkSpacePointFormationTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES
                     AthenaBaseComps
                     GaudiKernel
                     ActsTrkEvent
                     ActsTrkToolInterfacesLib
                     ActsUtilsLib
                     InDetCondTools
                     PixelReadoutGeometryLib
                     ReadoutGeometryBase
                     SCT_ReadoutGeometry
                     SiSpacePointFormationLib
                     xAODInDetMeasurement
                     )
