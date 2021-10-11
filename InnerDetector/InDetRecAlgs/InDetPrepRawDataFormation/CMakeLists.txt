# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetPrepRawDataFormation )

# Component(s) in the package:
atlas_add_component( InDetPrepRawDataFormation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthViews AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib AtlasDetDescr GaudiKernel IRegionSelector Identifier InDetByteStreamErrors InDetConditionsSummaryService InDetIdentifier InDetPrepRawData InDetRawData SCT_ConditionsData SiClusterizationToolLib StoreGateLib TRT_DriftCircleToolLib TrigSteeringEvent TrkSpacePoint ViewAlgsLib )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
