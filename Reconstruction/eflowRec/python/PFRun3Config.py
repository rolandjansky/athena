# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def PFCfg(inputFlags,**kwargs):

    #This is monolithic for now. 
    #Once a first complete example runs, this will be split into small modular chunks.
    #Some such items may be best placed elsewehere (e.g. put magnetic field setup in magnetic field git folder etc)
    result=ComponentAccumulator()

    from StoreGate.StoreGateConf import StoreGateSvc
    result.addService(StoreGateSvc("DetectorStore"))

    #Alias calibrated topoclusters, if they exist already, such that overwrite won't fial
    from SGComps.AddressRemappingConfig import InputRenameCfg
    result.merge(InputRenameCfg("xAOD::CaloClusterContainer","CaloCalTopoClusters",""))

    #Setup up general geometry
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    result.merge(GeoModelCfg(inputFlags))

    #load folders needed for IBL
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(inputFlags,['/Indet/IBLDist'],'INDET_OFL'))
    
    #Setup SCT conditions
    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_AlignCondAlg
    result.addCondAlgo(SCT_AlignCondAlg(name = "SCT_AlignCondAlg",UseDynamicAlignFolders = False))

    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_DetectorElementCondAlg
    result.addCondAlgo(SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg"))

    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    
    from AthenaCommon import CfgGetter
    result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("PixelDetectorTool", checkType=True) ]
    result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("SCT_DetectorTool", checkType=True) ]

    #Setup TRT geometry
    from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
    trtDetectorTool = TRT_DetectorTool()
    result.getService("GeoModelSvc").DetectorTools += [ trtDetectorTool ]

    #Setup up material for inner detector
    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    result.getService("GeoModelSvc").DetectorTools += [ InDetServMatTool() ]
    
    #Setup up tracking geometry
    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc, geom_svc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)
    
    #load folders needed for Run2 ID alignment
    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    result.merge(addFoldersSplitOnline(inputFlags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))
    
    #Setup up muon geometry
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))    

    #Setup magnetic field service
    result.merge(addFolders(inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_OFL'))
    result.merge(addFolders(inputFlags,['/EXT/DCS/MAGNETS/SENSORDATA'],'DCS_OFL'))

    from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
    iovDbSvc=result.getService("IOVDbSvc")
    iovDbSvc.FoldersToMetaData+=['/GLOBAL/BField/Maps']

    from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
    kwargs.setdefault( "UseDCS", True )
    result.addService(MagField__AtlasFieldSvc("AtlasFieldSvc",**kwargs))

    #hard-code MC conditions tag needed for my ESD file - must be a better way? how to auto-configure?
    iovDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"

    #Configure topocluster algorithms
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    result.merge(CaloTopoClusterCfg(inputFlags))
    
    from eflowRec.eflowRecConf import PFLeptonSelector
    PFLeptonSelector=PFLeptonSelector("PFLeptonSelector")
    result.addEventAlgo(PFLeptonSelector)

    from eflowRec.eflowRecConf import PFTrackSelector
    PFTrackSelector=PFTrackSelector("PFTrackSelector")

    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())

    from eflowRec.eflowRecConf import eflowTrackCaloExtensionTool
    TrackCaloExtensionTool=eflowTrackCaloExtensionTool(TrackCaloExtensionTool=pcExtensionTool)

    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    TrackSelectionTool = InDet__InDetTrackSelectionTool("PFTrackSelectionTool")

    TrackSelectionTool.CutLevel = "TightPrimary"
    TrackSelectionTool.minPt = 500.0 
    
    PFTrackSelector.trackSelectionTool = TrackSelectionTool
    
    result.addEventAlgo(PFTrackSelector)

    from eflowRec.eflowRecConf import PFAlgorithm
    PFAlgorithm = PFAlgorithm("PFAlgorithm")

    from eflowRec.eflowRecConf import PFClusterSelectorTool
    PFClusterSelectorTool = PFClusterSelectorTool("PFClusterSelectorTool")
    PFClusterSelectorTool.clustersName="CaloTopoClusters"
    
    PFAlgorithm.PFClusterSelectorTool = PFClusterSelectorTool

    PFAlgorithm.SubtractionToolList  = []

    from eflowRec.eflowRecConf import PFCellLevelSubtractionTool
    PFCellLevelSubtractionTool = PFCellLevelSubtractionTool("PFCellLevelSubtractionTool")
    
    from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
    CellEOverPTool=eflowCellEOverPTool_mc12_JetETMiss()
     
    PFCellLevelSubtractionTool.eflowCellEOverPTool=CellEOverPTool
    PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = 1

    from eflowRec.eflowRecConf import PFTrackClusterMatchingTool
    MatchingTool = PFTrackClusterMatchingTool("CalObjBldMatchingTool")
    MatchingTool_Pull_02 = PFTrackClusterMatchingTool("MatchingTool_Pull_02")
    MatchingTool_Pull_015 = PFTrackClusterMatchingTool("MatchingTool_Pull_015")
     
    MatchingTool_Pull_015.TrackPositionType   = 'EM2EtaPhi' # str
    MatchingTool_Pull_015.ClusterPositionType = 'PlainEtaPhi' # str
    MatchingTool_Pull_015.DistanceType        = 'EtaPhiSquareDistance' # str
    MatchingTool_Pull_015.MatchCut = 0.15*0.15 # float
    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_015 = MatchingTool_Pull_015
     
    MatchingTool_Pull_02.TrackPositionType   = 'EM2EtaPhi' # str
    MatchingTool_Pull_02.ClusterPositionType = 'PlainEtaPhi' # str
    MatchingTool_Pull_02.DistanceType        = 'EtaPhiSquareDistance' # str
    MatchingTool_Pull_02.MatchCut = 0.2*0.2 # float
    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_02 = MatchingTool_Pull_02
     
    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = MatchingTool

    PFAlgorithm.SubtractionToolList += [PFCellLevelSubtractionTool]

    from eflowRec.eflowRecConf import PFRecoverSplitShowersTool
    PFRecoverSplitShowersTool = PFRecoverSplitShowersTool("PFRecoverSplitShowersTool")
    CellEOverPTool_Recover=eflowCellEOverPTool_mc12_JetETMiss("eflowCellEOverPTool_mc12_JetETMiss_Recover")
    PFRecoverSplitShowersTool.eflowCellEOverPTool=CellEOverPTool_Recover

    MatchingTool_Recover = PFTrackClusterMatchingTool()
    MatchingTool_Recover.TrackPositionType   = 'EM2EtaPhi' # str
    MatchingTool_Recover.ClusterPositionType = 'PlainEtaPhi' # str
    MatchingTool_Recover.DistanceType        = 'EtaPhiSquareDistance' # str
    MatchingTool_Recover.MatchCut = 0.2*0.2 # float
    PFRecoverSplitShowersTool.PFTrackClusterMatchingTool = MatchingTool_Recover

    from eflowRec.eflowRecConf import PFMomentCalculatorTool
    PFMomentCalculatorTool = PFMomentCalculatorTool("PFMomentCalculatorTool")

    from CaloRec.CaloTopoClusterConfig import getTopoMoments
    PFClusterMomentsMaker = getTopoMoments(inputFlags)

    PFMomentCalculatorTool.CaloClusterMomentsMaker = PFClusterMomentsMaker

    from eflowRec.eflowRecConf import PFClusterCollectionTool
    PFClusterCollectionTool_default = PFClusterCollectionTool("PFClusterCollectionTool")
 
    PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool_default
 
    PFAlgorithm.BaseToolList = [PFMomentCalculatorTool]

    from eflowRec.eflowRecConf import PFLCCalibTool
    PFLCCalibTool = PFLCCalibTool("PFLCCalibTool")
 
    from CaloRec.CaloTopoClusterConfig import caloTopoCoolFolderCfg
    result.merge(caloTopoCoolFolderCfg(inputFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(inputFlags,"totalNoise"))
    result.merge(CaloNoiseCondAlgCfg(inputFlags,"electronicNoise"))

    PFClusterCollectionTool_LCCalib = PFClusterCollectionTool("PFClusterCollectionTool_LCCalib")
    PFLCCalibTool.eflowRecClusterCollectionTool=PFClusterCollectionTool_LCCalib
    PFLCCalibTool.UseLocalWeight = False

    from CaloRec.CaloTopoClusterConfig import getTopoClusterLocalCalibTools    
    lcCalibToolList = getTopoClusterLocalCalibTools(inputFlags)

    PFLCCalibTool.CaloClusterLocalCalib=lcCalibToolList[0]
    PFLCCalibTool.CaloClusterLocalCalibOOCC=lcCalibToolList[1]                                                            
    PFLCCalibTool.CaloClusterLocalCalibOOCCPi0=lcCalibToolList[2]
    PFLCCalibTool.CaloClusterLocalCalibDM=lcCalibToolList[3]
    
    PFAlgorithm.BaseToolList += [PFLCCalibTool]

    result.addEventAlgo(PFAlgorithm)

    from eflowRec.eflowRecConf import PFOChargedCreatorAlgorithm
    PFOChargedCreatorAlgorithm = PFOChargedCreatorAlgorithm("PFOChargedCreatorAlgorithm")
    result.addEventAlgo(PFOChargedCreatorAlgorithm)

    from eflowRec.eflowRecConf import PFONeutralCreatorAlgorithm
    PFONeutralCreatorAlgorithm =  PFONeutralCreatorAlgorithm("PFONeutralCreatorAlgorithm")

    result.addEventAlgo(PFONeutralCreatorAlgorithm)
    
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
