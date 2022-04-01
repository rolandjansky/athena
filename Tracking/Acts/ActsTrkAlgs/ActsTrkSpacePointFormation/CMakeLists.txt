# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkSpacePointFormation )

# External dependencies:

atlas_add_component( ActsTrkSpacePointFormation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES
                     AthenaBaseComps
                     AthenaMonitoringKernelLib
                     GaudiKernel
                     ActsTrkEvent
                     ActsTrkToolInterfacesLib
                     BeamSpotConditionsData
                     MagFieldConditions
                     PixelReadoutGeometryLib
                     SCT_ReadoutGeometry
                     SiSpacePointFormationLib
                     xAODInDetMeasurement
                     )
