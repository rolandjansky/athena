# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

#This configures pflow + everything it needs
def PFFullCfg(inputFlags,**kwargs):
  
    result=ComponentAccumulator()

    StoreGateSvc=CompFactory.StoreGateSvc
    result.addService(StoreGateSvc("DetectorStore"))

    #Alias calibrated topoclusters, if they exist already, such that overwrite won't fial
    from SGComps.AddressRemappingConfig import InputRenameCfg
    result.merge(InputRenameCfg("xAOD::CaloClusterContainer","CaloCalTopoClusters",""))

    #setup magnetic field service
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))

    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    result.merge(CaloTopoClusterCfg(inputFlags))

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

    from OutputStreamAthenaPool.OutputStreamConfig import addToAOD, addToESD
    #PFlow requires electrons, photons, muons and taus in order to have valid links to them. So lets add these objects to the AOD and ESD                                            
    toESDAndAOD = [f"xAOD::ElectronContainer#Electrons",f"xAOD::ElectronAuxContainer#ElectronsAux."]
    toESDAndAOD += [f"xAOD::PhotonContainer#Photons",f"xAOD::PhotonAuxContainer#PhotonsAux."]
    toESDAndAOD += [f"xAOD::MuonContainer#Muons",f"xAOD::MuonAuxContainer#MuonsAux."]
    toESDAndAOD += [f"xAOD::TauJetContainer#TauJets",f"xAOD::TauJetAuxContainer#TauJetsAux."]

    result.merge(addToESD(inputFlags, toESDAndAOD))
    result.merge(addToAOD(inputFlags, toESDAndAOD))

    result.merge(PFCfg(inputFlags))
    return result

#Configures only the pflow algorithms and tools - to be used from RecExCommon to avoid
#conflicts or if you only want to configure just the pflow algorithms and tools
def PFCfg(inputFlags,**kwargs):

    result=ComponentAccumulator()

    #Configure the pflow algorithms
    PFLeptonSelectorFactory=CompFactory.PFLeptonSelector
    PFLeptonSelector = PFLeptonSelectorFactory("PFLeptonSelector") 
    PFLeptonSelector.selectElectrons=False
    PFLeptonSelector.selectMuons=False
    result.addEventAlgo(PFLeptonSelector)

    from eflowRec.PFCfg import PFTrackSelectorAlgCfg
    useCaching = True
    #If reading ESD/AOD do not make use of caching of track extrapolations.
    if (inputFlags.Input.Format == "POOL" and not ('StreamRDO' in inputFlags.Input.ProcessingTags or 'OutputStreamRDO' in inputFlags.Input.ProcessingTags)):
        useCaching = False
    result.merge(PFTrackSelectorAlgCfg(inputFlags,"PFTrackSelector",useCaching))

    from eflowRec.PFCfg import getOfflinePFAlgorithm
    result.merge(getOfflinePFAlgorithm(inputFlags))

    # old PFO algorithm, keep gated behind a joboption but expect this is deprecated.    
    if(inputFlags.PF.useOldPFO):
        from eflowRec.PFCfg import getChargedPFOCreatorAlgorithm,getNeutralPFOCreatorAlgorithm
        result.addEventAlgo(getChargedPFOCreatorAlgorithm(inputFlags,""))
        result.addEventAlgo(getNeutralPFOCreatorAlgorithm(inputFlags,""))

    from eflowRec.PFCfg import getChargedFlowElementCreatorAlgorithm,getNeutralFlowElementCreatorAlgorithm,getLCNeutralFlowElementCreatorAlgorithm
    result.addEventAlgo(getChargedFlowElementCreatorAlgorithm(inputFlags,""))
    result.addEventAlgo(getNeutralFlowElementCreatorAlgorithm(inputFlags,""))
    result.addEventAlgo(getLCNeutralFlowElementCreatorAlgorithm(inputFlags,""))

    #Only do linking if not in eoverp mode
    if (not inputFlags.PF.EOverPMode):
      #Currently we do not have egamma reco in the run 3 config and hence there are no electrons/photons if not running from ESD or AOD
      #So in new config only schedule from ESD/AOD, in old config always schedule it if requested
      if (inputFlags.PF.useElPhotLinks and (inputFlags.Input.Format == "POOL" or inputFlags.PF.useRecExCommon)):
          from eflowRec.PFCfg import getEGamFlowElementAssocAlgorithm        
          result.addEventAlgo(getEGamFlowElementAssocAlgorithm(inputFlags))
    
      #Currently we do not have muon reco in the run 3 config and hence there are no muons if not running from ESD or AOD
      #So in new config only schedule from ESD/AOD, in old config always schedule it if requested it
      if (inputFlags.PF.useMuLinks and ((inputFlags.Input.Format == "POOL" and not ('StreamRDO' in inputFlags.Input.ProcessingTags or 'OutputStreamRDO' in inputFlags.Input.ProcessingTags)) or inputFlags.PF.useRecExCommon)):
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

    result.merge(addToESD(inputFlags, toESDAndAOD))
    result.merge(addToAOD(inputFlags, toESDAndAOD))

    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
      for el in result._allSequences:
        el.name = "TopAlg"

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
    cfg.merge(PFFullCfg(cfgFlags))
    
    from eflowRec.PFRun3Remaps import ListRemaps

    list_remaps=ListRemaps()
    for mapping in list_remaps:
        cfg.merge(mapping)    

    cfg.run()
