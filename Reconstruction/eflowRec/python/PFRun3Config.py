# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def getPFTrackSelectorAlgorithm(inputFlags):
    from eflowRec.eflowRecConf import PFTrackSelector
    PFTrackSelector=PFTrackSelector("PFTrackSelector")

    from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    extrapCfg = AtlasExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = extrapCfg.popPrivateTools())

    from eflowRec.eflowRecConf import eflowTrackCaloExtensionTool
    TrackCaloExtensionTool=eflowTrackCaloExtensionTool(TrackCaloExtensionTool=pcExtensionTool)

    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    TrackSelectionTool = InDet__InDetTrackSelectionTool("PFTrackSelectionTool")

    TrackSelectionTool.CutLevel = "TightPrimary"
    TrackSelectionTool.minPt = 500.0 
    
    PFTrackSelector.trackSelectionTool = TrackSelectionTool

    return PFTrackSelector

def getPFTrackClusterMatchingTool(inputFlags,matchCut,distanceType,clusterPositionType,name):
    from eflowRec.eflowRecConf import PFTrackClusterMatchingTool
    MatchingTool = PFTrackClusterMatchingTool(name)
    MatchingTool.ClusterPositionType = clusterPositionType
    MatchingTool.DistanceType = distanceType
    MatchingTool.MatchCut = matchCut*matchCut
    return MatchingTool
    
def getPFCellLevelSubtractionTool(inputFlags):
    from eflowRec.eflowRecConf import PFCellLevelSubtractionTool
    PFCellLevelSubtractionTool = PFCellLevelSubtractionTool("PFCellLevelSubtractionTool")
    
    from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
    PFCellLevelSubtractionTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss()

    if(True is inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.CalcEOverP = True
        PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = -1
    else:
        PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = 1

    if(True is inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","CalObjBldMatchingTool")
    else:
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,1.64,"EtaPhiSquareSignificance","GeomCenterEtaPhi","CalObjBldMatchingTool")

    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_015 = getPFTrackClusterMatchingTool(inputFlags,0.15,"EtaPhiSquareDistance","PlainEtaPhi","MatchingTool_Pull_015")
    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_02 = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","MatchingTool_Pull_02")            
    
    return PFCellLevelSubtractionTool

def getPFRecoverSplitShowersTool(inputFlags):
    from eflowRec.eflowRecConf import PFRecoverSplitShowersTool
    PFRecoverSplitShowersTool = PFRecoverSplitShowersTool("PFRecoverSplitShowersTool")

    from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
    PFRecoverSplitShowersTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss("eflowCellEOverPTool_mc12_JetETMiss_Recover")

    if (True is inputFlags.PF.recoverIsolatedTracks):
        PFRecoverSplitShowersTool.RecoverIsolatedTracks = True

    PFRecoverSplitShowersTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","MatchingToolRecover")

    return PFRecoverSplitShowersTool

def getPFClusterSelectorTool():
    from eflowRec.eflowRecConf import PFClusterSelectorTool
    PFClusterSelectorTool = PFClusterSelectorTool("PFClusterSelectorTool")
    PFClusterSelectorTool.clustersName="CaloTopoClusters"

    return PFClusterSelectorTool

def getPFMomentCalculatorTool(inputFlags):
    from eflowRec.eflowRecConf import PFMomentCalculatorTool
    PFMomentCalculatorTool = PFMomentCalculatorTool("PFMomentCalculatorTool")

    from CaloRec.CaloTopoClusterConfig import getTopoMoments
    PFMomentCalculatorTool.CaloClusterMomentsMaker = getTopoMoments(inputFlags)

    from eflowRec.eflowRecConf import PFClusterCollectionTool
    PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool("PFClusterCollectionTool")

    if(True is inputFlags.PF.useCalibHitTruthMoments):
        PFMomentCalculatorTool.UseCalibHitTruth=True
        from CaloRec.CaloTopoClusterConfig import getTopoCalibMoments
        PFMomentCalculatorTool.CaloCalibClusterMomentsMaker2 = getTopoCalibMoments(inputFlags)

    return PFMomentCalculatorTool

def getPFLCCalibTool(inputFlags):
    from eflowRec.eflowRecConf import PFLCCalibTool
    PFLCCalibTool = PFLCCalibTool("PFLCCalibTool")

    from eflowRec.eflowRecConf import PFClusterCollectionTool
    PFLCCalibTool.eflowRecClusterCollectionTool = PFClusterCollectionTool("PFClusterCollectionTool_LCCalib")
    PFLCCalibTool.UseLocalWeight = False

    from CaloRec.CaloTopoClusterConfig import getTopoClusterLocalCalibTools    
    lcCalibToolList = getTopoClusterLocalCalibTools(inputFlags)

    PFLCCalibTool.CaloClusterLocalCalib=lcCalibToolList[0]
    PFLCCalibTool.CaloClusterLocalCalibOOCC=lcCalibToolList[1]                                                            
    PFLCCalibTool.CaloClusterLocalCalibOOCCPi0=lcCalibToolList[2]
    PFLCCalibTool.CaloClusterLocalCalibDM=lcCalibToolList[3]

    return PFLCCalibTool

def getPFAlgorithm(inputFlags):

    from eflowRec.eflowRecConf import PFAlgorithm
    PFAlgorithm = PFAlgorithm("PFAlgorithm")   
    
    PFAlgorithm.PFClusterSelectorTool = getPFClusterSelectorTool()

    PFAlgorithm.SubtractionToolList = [getPFCellLevelSubtractionTool(inputFlags)]

    if(False is inputFlags.PF.EOverPMode):        
        PFAlgorithm.SubtractionToolList += [getPFRecoverSplitShowersTool(inputFlags)]
        
    PFAlgorithm.BaseToolList = [getPFMomentCalculatorTool(inputFlags)]
    PFAlgorithm.BaseToolList += [getPFLCCalibTool(inputFlags)]

    return PFAlgorithm

def getPFOCreators(inputFlags):
    from eflowRec.eflowRecConf import PFOChargedCreatorAlgorithm
    PFOChargedCreatorAlgorithm = PFOChargedCreatorAlgorithm("PFOChargedCreatorAlgorithm")
    if(True is inputFlags.PF.EOverPMode):
        PFOChargedCreatorAlgorithm.PFOOutputName="EOverPChargedParticleFlowObjects"

    from eflowRec.eflowRecConf import PFONeutralCreatorAlgorithm
    PFONeutralCreatorAlgorithm =  PFONeutralCreatorAlgorithm("PFONeutralCreatorAlgorithm")
    if(True is inputFlags.PF.EOverPMode):
        PFONeutralCreatorAlgorithm.PFOOutputName="EOverPNeutralParticleFlowObjects"
    if(True is inputFlags.PF.useCalibHitTruthMoments):
        PFONeutralCreatorAlgorithm.UseCalibHitTruth=True

    return PFOChargedCreatorAlgorithm, PFONeutralCreatorAlgorithm

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
    #These two lines fix ATLASRECTS-5053. I expect eventually we can remove them, once the underlying issue is fixed.
    trtDetectorTool.DoXenonArgonMixture = False
    trtDetectorTool.DoKryptonMixture = False
    result.getService("GeoModelSvc").DetectorTools += [ trtDetectorTool ]

    #Setup up material for inner detector
    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    result.getService("GeoModelSvc").DetectorTools += [ InDetServMatTool() ]
    
    #Setup up tracking geometry
    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
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

    iovDbSvc=result.getService("IOVDbSvc")

    from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
    kwargs.setdefault( "UseDCS", True )
    result.addService(MagField__AtlasFieldSvc("AtlasFieldSvc",**kwargs))

    #hard-code MC conditions tag needed for my ESD file - must be a better way? how to auto-configure?
    iovDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"

    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    result.merge(CaloTopoClusterCfg(inputFlags,doLCCalib=True))
    
    from CaloRec.CaloTopoClusterConfig import caloTopoCoolFolderCfg
    result.merge(caloTopoCoolFolderCfg(inputFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(inputFlags,"totalNoise"))
    result.merge(CaloNoiseCondAlgCfg(inputFlags,"electronicNoise"))

    #Configure the pflow algorithms
    from eflowRec.eflowRecConf import PFLeptonSelector
    result.addEventAlgo(PFLeptonSelector("PFLeptonSelector"))
    
    result.addEventAlgo(getPFTrackSelectorAlgorithm(inputFlags))

    result.addEventAlgo(getPFAlgorithm(inputFlags))

    PFOChargedCreatorAlgorithm, PFONeutralCreatorAlgorithm = getPFOCreators(inputFlags)
    result.addEventAlgo(PFOChargedCreatorAlgorithm)
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
