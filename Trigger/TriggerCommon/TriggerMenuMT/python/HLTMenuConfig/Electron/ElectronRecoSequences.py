#
#  Copyright (C) 2002-2010 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def l2CaloRecoCfg( flags ):
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco

    reco = InViewReco('FastCaloEMReco', roisKey='EMCaloRoIs')
    #    algAcc, alg = l2CaloAlgCfg( flags, roisKey = reco.name+'RoIs' )
    from TrigT2CaloEgamma.TrigT2CaloEgammaMTConfig import fastL2EgammaClusteringAlg
    ca = ComponentAccumulator()
    ca.addEventAlgo( CompFactory.AthViews.ViewDataVerifier(name='VDVFastEgammaCalo',
                                                           DataObjects = [('TrigRoiDescriptorCollection', 'StoreGateSvc+EMCaloRoIs'),
                                                                          ('CaloBCIDAverage', 'StoreGateSvc+CaloBCIDAverage') ]) )         
    ca.merge( fastL2EgammaClusteringAlg( flags, roisKey='EMCaloRoIs', doRinger=True) )
    reco.mergeReco( ca )
    return reco

def l2CaloHypoCfg( flags, name='UNSPECIFIED', CaloClusters='UNSPECIFIED' ):
    l2CaloHypo                     = CompFactory.TrigEgammaFastCaloHypoAlgMT( name )
    l2CaloHypo.CaloClusters        = CaloClusters
    return l2CaloHypo

