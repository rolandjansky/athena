# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiSpacePointFormation )

# Component(s) in the package:
atlas_add_library( SiSpacePointFormationLib
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePointFormation
                   LINK_LIBRARIES AthContainers AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib BeamSpotConditionsData GaudiKernel GeoPrimitives Identifier InDetPrepRawData ReadoutGeometryBase InDetReadoutGeometry SiSpacePointToolLib StoreGateLib TrkSpacePoint
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr xAODEventInfo InDetIdentifier )

atlas_add_component( SiSpacePointFormation
                     src/components/*.cxx
                     LINK_LIBRARIES SiSpacePointFormationLib )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
