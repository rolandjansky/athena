#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigEDMConfig.TriggerEDMRun3 import recordable

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)



def l2PhotonAlgCfg( flags ):
    acc = ComponentAccumulator()
    #from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1
    from AthenaConfiguration.ComponentFactory import CompFactory

    photonFex= CompFactory.TrigEgammaFastPhotonReAlgo("EgammaFastPhotonFex_1")
    #photonFex= L2PhotonFex_1()
    photonFex.TrigEMClusterName = recordable("HLT_FastCaloEMClusters")
    photonFex.PhotonsName = recordable("HLT_FastPhotons")
    photonFex.RoIs = "L2PhotonRecoRoIs"

    return acc, photonFex


def photonViewDataVerifierCfg():
    from AthenaConfiguration.ComponentFactory import CompFactory
    moveClusters = CompFactory.AthViews.ViewDataVerifier("VDVFastPhoton")
    moveClusters.DataObjects = [ ('xAOD::TrigEMClusterContainer','StoreGateSvc+HLT_FastCaloEMClusters'),
                                 ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+L2PhotonRecoRoIs' )]

    result = ComponentAccumulator()
    result.addEventAlgo(moveClusters)
    return result

def l2PhotonRecoCfg( flags ):
    from TriggerMenuMT.HLT.Menu.MenuComponents import InViewRecoCA

    reco = InViewRecoCA("L2PhotonReco")
    reco.inputMaker().RequireParentView = True
    reco.inputMaker().RoIsLink="initialRoI"

    moveClustersCfg = photonViewDataVerifierCfg()
    reco.mergeReco( moveClustersCfg )

    algAcc, alg = l2PhotonAlgCfg( flags )

    l2PhotonAlgAcc = ComponentAccumulator()
    l2PhotonAlgAcc.addEventAlgo(alg)
    
    reco.mergeReco( l2PhotonAlgAcc )
    reco.merge( algAcc )

    return reco

def l2PhotonHypoCfg( flags, Photons='Unspecified', RunInView=True):
    from AthenaConfiguration.ComponentFactory import CompFactory

    l2PhotonHypo = CompFactory.TrigEgammaFastPhotonHypoAlg()
    l2PhotonHypo.Photons = Photons
    l2PhotonHypo.RunInView = RunInView

    return l2PhotonHypo


def generatePhotonsCfg( flags ):

    acc = ComponentAccumulator()
    from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastCaloHypoAlg
    l2CaloHypo              = TrigEgammaFastCaloHypoAlg("EgammaFastPhotonCaloHypo")
    l2CaloHypo.CaloClusters = 'L2CaloEMClusters'


    from TriggerMenuMT.HLT.Electron.ElectronRecoSequences import l2CaloRecoCfg
    
    
    l2CaloReco = RecoFragmentsPool.retrieve( l2CaloRecoCfg, flags )
    acc.merge( l2CaloReco )
    
    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigEgammaFastCaloHypoToolFromDict )

    fastCaloStep = ChainStep( "Photon_step1", [fastCaloSequence] )



    l2PhotonReco = RecoFragmentsPool.retrieve( l2PhotonRecoCfg, flags )
    acc.merge( l2PhotonReco )
    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastPhotonHypoAlg
    l2PhotonHypo = TrigEgammaFastPhotonHypoAlg()
    l2PhotonHypo.Photons = "L2Photons"
    l2PhotonHypo.RunInView=True

    from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool import TrigEgammaFastPhotonHypoToolFromDict

    l2PhotonSequence = MenuSequence( Sequence    = l2PhotonReco.sequence(),
                                     Maker       = l2PhotonReco.inputMaker(),
                                     Hypo        = l2PhotonHypo,
                                     HypoToolGen = TrigEgammaFastPhotonHypoToolFromDict )

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
