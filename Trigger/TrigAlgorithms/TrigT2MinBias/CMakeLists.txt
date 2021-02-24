# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT2MinBias )

# Component(s) in the package:
atlas_add_component( TrigT2MinBias
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthViews AthenaBaseComps AthenaMonitoringKernelLib CaloEvent CaloIdentifier
                     DecisionHandlingLib GaudiKernel InDetIdentifier InDetPrepRawData StoreGateLib TileByteStreamLib TileConditionsLib TileEvent TileIdentifier TrigCompositeUtilsLib TrigT2CaloCommonLib TrkSpacePoint xAODTrigMinBias xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
