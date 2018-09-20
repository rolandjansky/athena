#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import DEBUG,VERBOSE

def l2PhotonCaloStepCfg( flags, chains ):

    from TrigUpgradeTest.MenuComponents import FilterHypoSequence
    fhSeq = FilterHypoSequence( 'PhotonFastCalo' )
    fhSeq.addFilter( chains, inKey = 'EMRoIDecisions' )

    from TrigUpgradeTest.ElectronMenuConfig import l2CaloRecoCfg # generator for the L2 Calo EM clustering
    from TrigUpgradeTest.MenuComponents import RecoFragmentsPool 

    # obtain the reconstruction CF fragment    
    fhSeq.addReco( RecoFragmentsPool.retrieve( l2CaloRecoCfg, flags ) )

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    hypo = TrigL2CaloHypoAlgMT("L2PhotonCaloHypo")
    hypo.CaloClusters        = 'L2CaloEMClusters'
    hypo.OutputLevel = DEBUG
    

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName    
    for chain in chains:
        tool  = TrigL2CaloHypoToolFromName( chain, chain )
        hypo.HypoTools +=  [ tool ]
        
    fhSeq.addHypo( hypo )
    
    return fhSeq


def l2PhotonAlgCfg( flags ):
    acc = ComponentAccumulator()
    from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1

    photonFex= L2PhotonFex_1()
    photonFex.TrigEMClusterName = "L2CaloEMClusters"
    photonFex.PhotonsName = "L2Photons"
    photonFex.RoIs = "L2PhotonRecoRoIs"
    photonFex.OutputLevel=DEBUG

    return acc, photonFex

def l2PhotonRecoCfg( flags ):
    from TrigUpgradeTest.MenuComponents import InViewReco

    reco = InViewReco("L2PhotonReco")
    reco.inputMaker().RequireParentView = True
    import AthenaCommon.CfgMgr as CfgMgr

    moveClusters = CfgMgr.AthViews__ViewDataVerifier("photonViewDataVerifier")
    moveClusters.DataObjects = [ ('xAOD::TrigEMClusterContainer','StoreGateSvc+L2CaloEMClusters') ]
    moveClusters.OutputLevel = DEBUG
    reco.addRecoAlg( moveClusters )

    algAcc, alg = l2PhotonAlgCfg( flags )
    reco.addRecoAlg( alg )
    reco.merge( algAcc )

    return reco

def l2PhotonRecoStepCfg( flags, chains, inputSequence ):
    from TrigUpgradeTest.MenuComponents import FilterHypoSequence
    fhSeq = FilterHypoSequence( 'PhotonFastReco' )
    fhSeq.addFilter( chains, inKey=inputSequence.hypoDecisions() )

    from TrigUpgradeTest.ElectronMenuConfig import l2CaloRecoCfg # generator for the L2 Calo EM clustering
    from TrigUpgradeTest.MenuComponents import RecoFragmentsPool 

    # obtain the reconstruction CF fragment
    fhSeq.addReco( RecoFragmentsPool.retrieve( l2PhotonRecoCfg, flags ) )

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
    hypo = TrigL2PhotonHypoAlgMT()
    hypo.Photons = "L2Photons"
    hypo.RunInView=True
    hypo.OutputLevel = VERBOSE

    from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromName
    for chain in chains:
        tool  = TrigL2PhotonHypoToolFromName( chain, chain )
        hypo.HypoTools +=  [ tool ]
        
    fhSeq.addHypo( hypo )
    
    return fhSeq



def generatePhotonsCfg( flags ):

    acc = ComponentAccumulator()
    photonChains = [ f.split()[0] for f in flags.Trigger.menu.photons ]
    if not photonChains:
        return None,None

    l2CaloStep = l2PhotonCaloStepCfg( flags, photonChains )
    acc.merge( l2CaloStep )

    l2PhotonStep = l2PhotonRecoStepCfg( flags, photonChains, l2CaloStep )
    acc.merge ( l2PhotonStep )

    return acc, [l2CaloStep, l2PhotonStep]


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.Trigger.menu.photons = ["HLT_g5_etcut L1_EM3", "HLT_g10_etcut L1_EM3"]
    ConfigFlags.lock()

    acc, sequences = generatePhotonsCfg( ConfigFlags )
    acc.printConfig()
