# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def getOfflinePFAlgorithm(inputFlags):
    result=ComponentAccumulator()

    PFAlgorithm=CompFactory.PFAlgorithm
    PFAlgorithm = PFAlgorithm("PFAlgorithm")

    from eflowRec.PFCfg import getPFClusterSelectorTool
    PFAlgorithm.PFClusterSelectorTool = getPFClusterSelectorTool("CaloTopoClusters","CaloCalTopoClusters","PFClusterSelectorTool")

    from eflowRec.PFCfg import getPFCellLevelSubtractionTool
    PFAlgorithm.SubtractionToolList = [getPFCellLevelSubtractionTool(inputFlags,"PFCellLevelSubtractionTool")]

    if(False is inputFlags.PF.EOverPMode):
        from eflowRec.PFCfg import getPFRecoverSplitShowersTool
        PFAlgorithm.SubtractionToolList += [getPFRecoverSplitShowersTool(inputFlags,"PFRecoverSplitShowersTool")]

    from eflowRec.PFCfg import getPFMomentCalculatorTool
    PFMomentCalculatorTools=result.popToolsAndMerge(getPFMomentCalculatorTool(inputFlags,[]))
    PFAlgorithm.BaseToolList = [PFMomentCalculatorTools]
    from eflowRec.PFCfg import getPFLCCalibTool
    PFAlgorithm.BaseToolList += [getPFLCCalibTool(inputFlags)]
    result.addEventAlgo(PFAlgorithm)
    return result

def PFTauFlowElementLinkingCfg(inputFlags,neutral_FE_cont_name="",charged_FE_cont_name="",AODTest=False):
    result=ComponentAccumulator()

    from eflowRec.PFCfg import getTauFlowElementAssocAlgorithm
    result.addEventAlgo(getTauFlowElementAssocAlgorithm(inputFlags,neutral_FE_cont_name,charged_FE_cont_name,AODTest))

    return result

def PFCfg(inputFlags,**kwargs):

    #This is monolithic for now.
    #Once a first complete example runs, this will be split into small modular chunks.
    #Some such items may be best placed elsewehere (e.g. put magnetic field setup in magnetic field git folder etc)
    result=ComponentAccumulator()

    StoreGateSvc=CompFactory.StoreGateSvc
    result.addService(StoreGateSvc("DetectorStore"))

    #Alias calibrated topoclusters, if they exist already, such that overwrite won't fial
    from SGComps.AddressRemappingConfig import InputRenameCfg
    result.merge(InputRenameCfg("xAOD::CaloClusterContainer","CaloCalTopoClusters",""))

    #Setup up general geometry
    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(inputFlags))

    #Setup TRT conditions
    TRTAlignCondAlg=CompFactory.TRTAlignCondAlg
    result.addCondAlgo(TRTAlignCondAlg(name = "TRTAlignCondAlg",UseDynamicFolders = inputFlags.GeoModel.Align.Dynamic))

    #Setup Pixel conditions
    PixelAlignCondAlg=CompFactory.PixelAlignCondAlg
    result.addCondAlgo(PixelAlignCondAlg(name = "PixelAlignCondAlg",UseDynamicAlignFolders = inputFlags.GeoModel.Align.Dynamic))
    
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg
    result.merge(PixelDetectorElementCondAlgCfg(inputFlags))

    #Setup SCT conditions
    SCT_AlignCondAlg=CompFactory.SCT_AlignCondAlg
    result.addCondAlgo(SCT_AlignCondAlg(name = "SCT_AlignCondAlg",UseDynamicAlignFolders = inputFlags.GeoModel.Align.Dynamic))

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_DetectorElementCondAlgCfg
    result.merge(SCT_DetectorElementCondAlgCfg(inputFlags))

    GeometryDBSvc=CompFactory.GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))

    #from AthenaCommon import CfgGetter
    #result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("PixelDetectorTool", checkType=True) ]
    #result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("SCT_DetectorTool", checkType=True) ]

    #Setup TRT geometry
    TRT_DetectorTool=CompFactory.TRT_DetectorTool
    trtDetectorTool = TRT_DetectorTool()
    #These two lines fix ATLASRECTS-5053. I expect eventually we can remove them, once the underlying issue is fixed.
    trtDetectorTool.DoXenonArgonMixture = False
    trtDetectorTool.DoKryptonMixture = False
    result.getService("GeoModelSvc").DetectorTools += [ trtDetectorTool ]

    #Setup up material for inner detector
    InDetServMatTool=CompFactory.InDetServMatTool
    result.getService("GeoModelSvc").DetectorTools += [ InDetServMatTool() ]

    #Setup up tracking geometry
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    #load folders needed for Run2 ID alignment
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))

    #Setup up muon geometry
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))

    #setup magnetic field service
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))

    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    result.merge(CaloTopoClusterCfg(inputFlags,
                                    doLCCalib=True))


    #from CaloRec.CaloTopoClusterConfig import caloTopoCoolFolderCfg
    #result.merge(caloTopoCoolFolderCfg(inputFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(inputFlags,"totalNoise"))
    result.merge(CaloNoiseCondAlgCfg(inputFlags,"electronicNoise"))

    #Cache the track extrapolations
    from TrackToCalo.CaloExtensionBuilderAlgCfg import CaloExtensionBuilderAlgCfg
    result.merge(CaloExtensionBuilderAlgCfg(inputFlags))

    #Configure the pflow algorithms
    PFLeptonSelector=CompFactory.PFLeptonSelector
    result.addEventAlgo(PFLeptonSelector("PFLeptonSelector"))

    from eflowRec.PFCfg import PFTrackSelectorAlgCfg
    result.merge(PFTrackSelectorAlgCfg(inputFlags,"PFTrackSelector"))

    result.merge(getOfflinePFAlgorithm(inputFlags))

    from eflowRec.PFCfg import getChargedPFOCreatorAlgorithm,getNeutralPFOCreatorAlgorithm
    result.addEventAlgo(getChargedPFOCreatorAlgorithm(inputFlags,""))
    result.addEventAlgo(getNeutralPFOCreatorAlgorithm(inputFlags,""))

    from eflowRec.PFCfg import getChargedFlowElementCreatorAlgorithm,getNeutralFlowElementCreatorAlgorithm
    result.addEventAlgo(getChargedFlowElementCreatorAlgorithm(inputFlags,""))
    result.addEventAlgo(getNeutralFlowElementCreatorAlgorithm(inputFlags,""))

    from eflowRec.PFCfg import getMuonFlowElementAssocAlgorithm,getEGamFlowElementAssocAlgorithm
    result.addEventAlgo(getMuonFlowElementAssocAlgorithm(inputFlags))
    result.addEventAlgo(getEGamFlowElementAssocAlgorithm(inputFlags))

    return result

if __name__=="__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]
    cfgFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg=MainServicesCfg(cfgFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))
    cfg.merge(PFCfg(cfgFlags))
    
    from eflowRec.PFRun3Remaps import ListRemaps

    list_remaps=ListRemaps()
    for mapping in list_remaps:
        cfg.merge(mapping)

    cfg.run()
