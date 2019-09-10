#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def l2CaloRecoCfg( flags ):
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco

    reco = InViewReco("FastCaloEMReco")
    #    algAcc, alg = l2CaloAlgCfg( flags, roisKey = reco.name+'RoIs' )
    from TrigT2CaloEgamma.TrigT2CaloEgammaMTConfig import fastL2EgammaClusteringAlg
    algAcc = fastL2EgammaClusteringAlg( flags, roisKey=reco.name+'RoIs')
    reco.mergeReco( algAcc )
    return reco

def l2CaloHypoCfg( flags, name="UNSPECIFIED", CaloClusters="UNSPECIFIED" ):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    l2CaloHypo                     = TrigL2CaloHypoAlgMT( name )
    l2CaloHypo.CaloClusters        = CaloClusters
    return l2CaloHypo

