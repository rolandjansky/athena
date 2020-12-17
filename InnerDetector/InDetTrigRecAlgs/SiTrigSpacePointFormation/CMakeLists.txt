# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiTrigSpacePointFormation )

include_directories(src)

# Component(s) in the package:
atlas_add_component( SiTrigSpacePointFormation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps BeamSpotConditionsData EventPrimitives GaudiKernel IRegionSelector Identifier InDetIdentifier InDetPrepRawData ReadoutGeometryBase InDetReadoutGeometry InDetTrigToolInterfacesLib SiSpacePointFormationLib SiSpacePointToolLib StoreGateLib TrigInterfacesLib TrigSteeringEvent TrigTimeAlgsLib TrkSpacePoint )

# Install files from the package:
atlas_install_python_modules( python/*.py )
