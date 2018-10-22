# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg    
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperTool
from StoreGate.StoreGateConf import StoreGateSvc

def MuonGeoModelCfg(flags):
    acc = ComponentAccumulator()

    # get the GeoModelSvc and add MuonDetectorTool
    gmsAcc,gms=GeoModelCfg(flags )
    acc.merge(gmsAcc)

    detTool = MuonDetectorTool()
    detTool.UseConditionDb = 1
    detTool.UseIlinesFromGM = 1    
    gms.DetectorTools += [ detTool ]
    acc.addService(gms)

    # Temporary, until we move to services/private tools
    acc.addPublicTool( Muon__MuonIdHelperTool() )

    return acc
