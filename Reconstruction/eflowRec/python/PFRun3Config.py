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

    #Setup up tracking geometry
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

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
    # FIXME: This inversion to merge in CAs is a workaround, which can be removed once SiDetElementCondAlgs 
    # don't depend on Muons/TRT/alignment/otherSiSubdetectorAlignment anymore.
    tempCA = CaloExtensionBuilderAlgCfg(inputFlags)
    tempCA.merge(result)
    result = tempCA

    #Configure the pflow algorithms
    PFLeptonSelectorFactory=CompFactory.PFLeptonSelector
    PFLeptonSelector = PFLeptonSelectorFactory("PFLeptonSelector") 
    PFLeptonSelector.selectElectrons=False
    PFLeptonSelector.selectMuons=False
    result.addEventAlgo(PFLeptonSelector)

    from eflowRec.PFCfg import PFTrackSelectorAlgCfg
    result.merge(PFTrackSelectorAlgCfg(inputFlags,"PFTrackSelector"))

    result.merge(getOfflinePFAlgorithm(inputFlags))

    # old PFO algorithm, keep gated behind a joboption but expect this is deprecated.    
    if(inputFlags.PF.useOldPFO):
        from eflowRec.PFCfg import getChargedPFOCreatorAlgorithm,getNeutralPFOCreatorAlgorithm
        result.addEventAlgo(getChargedPFOCreatorAlgorithm(inputFlags,""))
        result.addEventAlgo(getNeutralPFOCreatorAlgorithm(inputFlags,""))

    from eflowRec.PFCfg import getChargedFlowElementCreatorAlgorithm,getNeutralFlowElementCreatorAlgorithm
    result.addEventAlgo(getChargedFlowElementCreatorAlgorithm(inputFlags,""))
    result.addEventAlgo(getNeutralFlowElementCreatorAlgorithm(inputFlags,""))

    #Currently we do not have egamma reco in the run 3 config and hence there are no electrons/photons if not running from ESD or AOD
    if(inputFlags.PF.useElPhotLinks and inputFlags.Input.Format == "POOL" ):
        from eflowRec.PFCfg import getEGamFlowElementAssocAlgorithm        
        result.addEventAlgo(getEGamFlowElementAssocAlgorithm(inputFlags))
    
    #Currently we do not have muon reco in the run 3 config and hence there are no muons if not running from ESD or AOD
    if(inputFlags.PF.useMuLinks and inputFlags.Input.Format == "POOL" ):
        from eflowRec.PFCfg import getMuonFlowElementAssocAlgorithm
        result.addEventAlgo(getMuonFlowElementAssocAlgorithm(inputFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import addToAOD, addToESD
    toESDAndAOD = ""
    if(inputFlags.PF.EOverPMode):
      toESDAndAOD = [f"xAOD::FlowElementContainer#EOverPChargedParticleFlowObjects",f"xAOD::FlowElementAuxContainer#EOverPChargedParticleFlowObjectsAux."]
      toESDAndAOD += [f"xAOD::FlowElementContainer#EOverPNeutralParticleFlowObjects",f"xAOD::FlowElementAuxContainer#EOverPNeutralParticleFlowObjectsAux."]
    else:
      toESDAndAOD = [f"xAOD::FlowElementContainer#JetETMissChargedParticleFlowObjects", f"xAOD::FlowElementAuxContainer#JetETMissChargedParticleFlowObjectsAux."]
      toESDAndAOD += [f"xAOD::FlowElementContainer#JetETMissNeutralParticleFlowObjects",f"xAOD::FlowElementAuxContainer#JetETMissNeutralParticleFlowObjectsAux.-FEShowerSubtractedClusterLink."]
      toESDAndAOD += [f"xAOD::FlowElementContainer#JetETMissLCNeutralParticleFlowObjects",f"xAOD::ShallowAuxContainer#JetETMissLCNeutralParticleFlowObjectsAux."]

    #PFlow requires electrons, photons, muons and taus in order to have valid links to them. So lets add these objects to the AOD and ESD
    toESDAndAOD += [f"xAOD::ElectronContainer#Electrons",f"xAOD::ElectronAuxContainer#ElectronsAux."]
    toESDAndAOD += [f"xAOD::PhotonContainer#Photons",f"xAOD::PhotonAuxContainer#PhotonsAux."]
    toESDAndAOD += [f"xAOD::MuonContainer#Muons",f"xAOD::MuonAuxContainer#MuonsAux."]
    toESDAndAOD += [f"xAOD::TauJetContainer#TauJets",f"xAOD::TauJetAuxContainer#TauJetsAux."]
  
    result.merge(addToESD(inputFlags, toESDAndAOD))
    result.merge(addToAOD(inputFlags, toESDAndAOD))

    return result

if __name__=="__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]
    cfgFlags.Output.AODFileName="output_AOD.root"
    cfgFlags.Output.doWriteAOD=True
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
