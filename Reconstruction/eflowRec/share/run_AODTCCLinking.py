#Matt A's custom python accumulator for R22 
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
    

def MyAlgConfigNeutral(inputFlags,**kwargs,):
    # this is based on the PFRun3Config, so parts of it are gonna be used as is
    result=ComponentAccumulator()

    StoreGateSvc=CompFactory.StoreGateSvc

    result.addService(StoreGateSvc("DetectorStore"))
    

    from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
    TCCReco=runTCCReconstruction(inputFlags, outputTCCName="TrackCaloClusters",doNeutral=True,doCharged=False,doCombined=False)
    result.merge(TCCReco)

    StoreGateSvc.Dump = True
    return result




def MyAlgConfigCharged(inputFlags,**kwargs,):
    # this is based on the PFRun3Config, so parts of it are gonna be used as is
    result=ComponentAccumulator()

    StoreGateSvc=CompFactory.StoreGateSvc

    result.addService(StoreGateSvc("DetectorStore"))
    


    from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
    TCCReco=runTCCReconstruction(inputFlags, outputTCCName="TrackCaloClusters",doNeutral=False,doCharged=True,doCombined=False)
    result.merge(TCCReco)
    
    StoreGateSvc.Dump = True
    return result


def LinkerAlgConfig(inputFlags,**kwargs):
    #link El/Phot/Muon to TrackCaloClusters
    CA=ComponentAccumulator()
    StoreGateSvc=CompFactory.StoreGateSvc
    CA.addService(StoreGateSvc("DetectorStore"))

    from eflowRec.PFCfg import getEGamFlowElementAssocAlgorithm
    from eflowRec.PFCfg import getMuonFlowElementAssocAlgorithm
    
    CA.addEventAlgo(getEGamFlowElementAssocAlgorithm(inputFlags,AODTest=False,doTCC=True))
    CA.addEventAlgo(getMuonFlowElementAssocAlgorithm(inputFlags,LinkNeutralFEClusters=False,useMuonTopoClusters=True,AODTest=False,doTCC=True))


    return CA


def TauLinkerConfig(inputFlags, **kwargs):
    #separate linker, can merge to main one for no loss of generality
    TauLinkerCA=ComponentAccumulator()
    # add Storegate
    StoreGateSvc=CompFactory.StoreGateSvc
    TauLinkerCA.addService(StoreGateSvc("DetectorStore"))
    
    from eflowRec.PFCfg import getTauFlowElementAssocAlgorithm
    TauLinkerCA.addEventAlgo(getTauFlowElementAssocAlgorithm(inputFlags,doTCC=True))
    return TauLinkerCA



if __name__=="__main__":
    #the "standard" main loop for Athena R22
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags
    from AthenaConfiguration.Enums import Format
    
    cfgFlags.Input.isMC=True
    cfgFlags.Input.Format=Format.POOL

    cfgFlags.Input.Files=["/scratch/anthony/GPF_CODE/TCC_STUDIES/CustomAlg/myAOD.root"]
    cfgFlags.Output.AODFileName="/scratch/anthony/GPF_CODE/TCC_STUDIES/CustomAlg/output_AOD.root"
    cfgFlags.Exec.MaxEvents=20
    cfgFlags.Output.doWriteAOD=True
    cfgFlags.Common.ProductionStep=""
    cfgFlags.dump()
    cfgFlags.lock()
    

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg=MainServicesCfg(cfgFlags)

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    inputList=["xAOD::TrackParticleContainer#InDetTrackParticles", "xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."]
    inputList.append("xAOD::FlowElementContainer#JetETMissChargedParticleFlowObjects")
    inputList.append("xAOD::FlowElementContainer#JetETMissNeutralParticleFlowObjects")
    inputList.append("xAOD::FlowElementAuxContainer#JetETMissNeutralParticleFlowObjectsAux.")
    inputList.append("xAOD::FlowElementAuxContainer#JetETMissChargedParticleFlowObjectsAux.")
    inputList.append("xAOD::FlowElementContainer#TrackCaloClustersCharged")
    inputList.append("xAOD::FlowElementAuxContainer#TrackCaloClustersChargedAux.")
    inputList.append("xAOD::FlowElementContainer#TrackCaloClustersNeutral")
    inputList.append("xAOD::FlowElementAuxContainer#TrackCaloClustersNeutralAux.")

    inputList.append("xAOD::MuonContainer#Muons")
    inputList.append("xAOD::MuonAuxContainer#*")
    inputList.append("xAOD::PhotonContainer#Photons")
    inputList.append("xAOD::PhotonAuxContainer#*")
    inputList.append("xAOD::ElectronContainer#Electrons")
    inputList.append("xAOD::ElectronAuxContainer#*")
    inputList.append("xAOD::TauJetContainer#*")
    inputList.append("xAOD::TauJetAuxContainer#*")
    
    
    cfg.merge(OutputStreamCfg(cfgFlags,"AOD",ItemList=inputList))
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    
    cfg.merge(PoolReadCfg(cfgFlags))
    cfg.merge(MyAlgConfigNeutral(cfgFlags))
    cfg.merge(MyAlgConfigCharged(cfgFlags))
    cfg.merge(LinkerAlgConfig(cfgFlags)) # El/Phot/Muon linkers
    #cfg.merge(TauLinkerConfig(cfgFlags)) # Doesn't yet work because of "passTauThinning" attrib not saved at AOD
    print("MASTER CA CONFIG")
    print(cfg)
    cfg.printConfig()

    cfg.run()

