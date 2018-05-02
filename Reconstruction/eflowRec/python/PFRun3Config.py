# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def PFCfg(inputFlags,**kwargs):

    #This is monolithic for now. 
    #Once a first complete example runs, this will be split into small modular chunks.
    #Some such items may be best placed elsewehere (e.g. put magnetic field setup in magnetic field git folder etc)
    result=ComponentAccumulator()

    from StoreGate.StoreGateConf import StoreGateSvc
    result.addService(StoreGateSvc("DetectorStore"))
    
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    result.addConfig(GeoModelCfg,inputFlags)

    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    
    from AthenaCommon import CfgGetter
    result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("PixelDetectorTool", checkType=True) ]
    result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("SCT_DetectorTool", checkType=True) ]

    from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
    trtDetectorTool = TRT_DetectorTool()
    result.getService("GeoModelSvc").DetectorTools += [ trtDetectorTool ]

    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    result.getService("GeoModelSvc").DetectorTools += [ InDetServMatTool() ]

    from IOVDbSvc.IOVDbSvcConfig import addFolders,IOVDbSvcCfg
    result.addConfig(addFolders,inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_OFL')
    result.addConfig(addFolders,inputFlags,['/EXT/DCS/MAGNETS/SENSORDATA'],'DCS_OFL')
    
    iovDbSvc=result.getService("IOVDbSvc")
    iovDbSvc.FoldersToMetaData+=['/GLOBAL/BField/Maps']

    from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
    kwargs.setdefault( "UseDCS", True )
    result.addService(MagField__AtlasFieldSvc("AtlasFieldSvc",**kwargs))

    #load folders needed for Run2 ID alignment
    result.addConfig(addFolders,inputFlags,['/Indet/Align'],'INDET_OFL')
    result.addConfig(addFolders,inputFlags,['/TRT/Align'],'TRT_OFL')

    #load folders needed for IBL
    result.addConfig(addFolders,inputFlags,['/Indet/IBLDist'],'INDET_OFL')

    #hard-code MC conditions tag needed for my ESD file - must be a better way? how to auto-configure?
    iovDbSvc.GlobalTag="OFLCOND-MC16-SDR-13"
    
    from eflowRec.eflowRecConf import PFLeptonSelector
    PFLeptonSelector=PFLeptonSelector("PFLeptonSelector")
    result.addEventAlgo(PFLeptonSelector)

    from eflowRec.eflowRecConf import PFTrackSelector
    PFTrackSelector=PFTrackSelector("PFTrackSelector")

    from eflowRec.eflowRecConf import eflowTrackCaloExtensionTool
    TrackCaloExtensionTool=eflowTrackCaloExtensionTool()

    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    TrackSelectionTool = InDet__InDetTrackSelectionTool("PFTrackSelectionTool")

    TrackSelectionTool.CutLevel = "TightPrimary"
    TrackSelectionTool.minPt = 500.0 
    
    PFTrackSelector.trackSelectionTool = TrackSelectionTool
    
    result.addEventAlgo(PFTrackSelector)

    return result

if __name__=="__main__":
    cfg=ComponentAccumulator()
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.set("global.isMC",True)
    cfgFlags.set("global.InputFiles",["/data/hodgkinson/scratchFiles/mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s2832_r7968/ESD.08355655._001904.pool.root.1"])
    cfgFlags.lock()
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.addConfig(PoolReadCfg,cfgFlags)
    
    cfg.addConfig(PFCfg,cfgFlags)

    f=open("PF.pkl","w")
    cfg.store(f)
    f.close()
    
