# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonRdoToPrepData )

# Component(s) in the package:
atlas_add_component( MuonRdoToPrepData
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib Identifier TrigSteeringEvent IRegionSelector MuonPrepRawData MuonTrigCoinData AthViews MuonCnvToolInterfacesLib MuonIdHelpersLib ViewAlgsLib )
