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
    result.mergeAll(GeoModelCfg(inputFlags))

    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc, geom_svc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))    

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
    result.merge(addFolders(inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_OFL'))
    result.merge(addFolders(inputFlags,['/EXT/DCS/MAGNETS/SENSORDATA'],'DCS_OFL'))
    
    iovDbSvc=result.getService("IOVDbSvc")
    iovDbSvc.FoldersToMetaData+=['/GLOBAL/BField/Maps']

    from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
    kwargs.setdefault( "UseDCS", True )
    result.addService(MagField__AtlasFieldSvc("AtlasFieldSvc",**kwargs))

    #load folders needed for Run2 ID alignment
    result.merge(addFolders(inputFlags,['/Indet/Align'],'INDET_OFL'))
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))
                 
    #load folders needed for IBL
    result.merge(addFolders(inputFlags,['/Indet/IBLDist'],'INDET_OFL'))

    #hard-code MC conditions tag needed for my ESD file - must be a better way? how to auto-configure?
    iovDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"
    
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

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    cfgFlags.lock()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    cfg=MainServicesSerialCfg() 

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))
    cfg.merge(PFCfg(cfgFlags))

    cfg.run()
