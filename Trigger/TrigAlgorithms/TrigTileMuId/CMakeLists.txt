# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTileMuId )

# Component(s) in the package:
atlas_add_component( TrigTileMuId
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel ByteStreamCnvSvcBaseLib ByteStreamData CaloIdentifier CxxUtils GaudiKernel GeneratorObjects MagFieldInterfaces RegionSelectorLib TileByteStreamLib TileEvent TrigInDetEvent TrigInDetToolInterfacesLib TrigInterfacesLib TrigMuonEvent TrigSteeringEvent TrigT1Interfaces TrigT2CaloCommonLib TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
