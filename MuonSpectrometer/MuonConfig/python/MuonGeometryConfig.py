# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperTool
from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
from MuonAGDD.MuonAGDDConf import MuonAGDDTool, NSWAGDDTool
from StoreGate.StoreGateConf import StoreGateSvc
import os

def MuonGeoModelCfg(flags):
    acc = ComponentAccumulator()

    # get the GeoModelSvc and add MuonDetectorTool
    gmsAcc,gms=GeoModelCfg(flags )
    acc.merge(gmsAcc)

    detTool = MuonDetectorTool()
    detTool.UseConditionDb = 1
    detTool.UseIlinesFromGM = 1
    if ( ( not flags.Detector.SimulateMuon or flags.Detector.OverlayMuon ) and "AthSimulation_DIR" not in os.environ ):
        # Needs configuration from MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonAlignConfig.py to be migrated
        #detTool.TheMuonAlignmentTool = "MuonAlignmentDbTool/MGM_AlignmentDbTool"
        pass
    else:
        detTool.TheMuonAlignmentTool = ""
        detTool.UseConditionDb = 0
        detTool.UseAsciiConditionData = 0
        if flags.Detector.SimulateMuon:
            detTool.FillCacheInitTime = 0
            if flags.GeoModel.Run=="RUN3" or flags.GeoModel.Run=="RUN4":
                MuonDetectorTool.StationSelection  = 2
                MuonDetectorTool.SelectedStations  = [ "EIL1", "EIL2", "EIL6", "EIL7",
                                                       "EIS*", "EIL10", "EIL11", "EIL12",
                                                       "EIL17", "CSS*", "CSL*", "T4E*",
                                                       "T4F*" ]
            ## Additional material in the muon system
            AGDD2Geo = AGDDtoGeoSvc()
            muonAGDDTool = MuonAGDDTool("MuonSpectrometer", BuildNSW=False)
            acc.addPublicTool(muonAGDDTool)
            AGDD2Geo.Builders += [ muonAGDDTool ]
            if flags.GeoModel.Run=="RUN3" or flags.GeoModel.Run=="RUN4":
                nswAGDDTool = NSWAGDDTool("NewSmallWheel", Locked=False)
                nswAGDDTool.Volumes = ["NewSmallWheel"]
                nswAGDDTool.DefaultDetector = "Muon"
                acc.addPublicTool(nswAGDDTool)
            acc.addService(AGDD2Geo)

    gms.DetectorTools += [ detTool ]
    acc.addService(gms)

    # Temporary, until we move to services/private tools
    acc.addPublicTool( Muon__MuonIdHelperTool() )

    return acc
