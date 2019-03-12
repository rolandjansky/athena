#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import DEBUG,VERBOSE
from TrigEDMConfig.TriggerEDMRun3 import recordable

def l2PhotonAlgCfg( flags ):
    acc = ComponentAccumulator()
    from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1

    photonFex= L2PhotonFex_1()
    photonFex.TrigEMClusterName = "L2CaloEMClusters"
    photonFex.PhotonsName = recordable("L2Photons")
    photonFex.RoIs = "L2PhotonRecoRoIs"
    photonFex.OutputLevel=DEBUG

    return acc, photonFex

def l2PhotonRecoCfg( flags ):
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco

    reco = InViewReco("L2PhotonReco")
    reco.inputMaker().RequireParentView = True
    reco.inputMaker().RoIsLink="roi"
    import AthenaCommon.CfgMgr as CfgMgr

    moveClusters = CfgMgr.AthViews__ViewDataVerifier("photonViewDataVerifier")
    moveClusters.DataObjects = [ ('xAOD::TrigEMClusterContainer','StoreGateSvc+L2CaloEMClusters') ]
    moveClusters.OutputLevel = DEBUG
    reco.addRecoAlg( moveClusters )

    algAcc, alg = l2PhotonAlgCfg( flags )
    reco.addRecoAlg( alg )
    reco.merge( algAcc )

    return reco

def l2PhotonHypoCfg( flags, Photons='Unspecified', RunInView=True):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT

    l2PhotonHypo = TrigL2PhotonHypoAlgMT()
    l2PhotonHypo.Photons = Photons
    l2PhotonHypo.RunInView = RunInView
    l2PhotonHypo.OutputLevel = VERBOSE

    return l2PhotonHypo


def generatePhotonsCfg( flags ):

    acc = ComponentAccumulator()
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    l2CaloHypo              = TrigL2CaloHypoAlgMT("L2PhotonCaloHypo")
    l2CaloHypo.CaloClusters = 'L2CaloEMClusters'


    from TrigUpgradeTest.ElectronMenuConfig import l2CaloRecoCfg
    
    
    l2CaloReco = RecoFragmentsPool.retrieve( l2CaloRecoCfg, flags )
    acc.merge( l2CaloReco )
    
    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromDict )

    fastCaloStep = ChainStep( "Photon_step1", [fastCaloSequence] )



    l2PhotonReco = RecoFragmentsPool.retrieve( l2PhotonRecoCfg, flags )
    acc.merge( l2PhotonReco )
    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
    l2PhotonHypo = TrigL2PhotonHypoAlgMT()
    l2PhotonHypo.Photons = "L2Photons"
    l2PhotonHypo.RunInView=True
    l2PhotonHypo.OutputLevel = VERBOSE

    from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromDict

    l2PhotonSequence = MenuSequence( Sequence    = l2PhotonReco.sequence(),
                                     Maker       = l2PhotonReco.inputMaker(),
                                     Hypo        = l2PhotonHypo,
                                     HypoToolGen = TrigL2PhotonHypoToolFromDict )

    l2PhotonStep = ChainStep( "Photon_step2", [ l2PhotonSequence] )
    


    chains = [ Chain(c.split()[0], c.split()[1], [fastCaloStep, l2PhotonStep] )  for c in flags.Trigger.menu.photons ]    


    

    return acc, chains


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.Trigger.menu.photons = ["HLT_g5_etcut L1_EM3", "HLT_g10_etcut L1_EM3"]
    ConfigFlags.lock()

    acc, sequences = generatePhotonsCfg( ConfigFlags )
    acc.printConfig()
