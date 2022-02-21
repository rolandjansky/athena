#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def l2CaloRecoCfg( flags ):
    from TriggerMenuMT.HLT.Config.MenuComponents import InViewRecoCA

    reco = InViewRecoCA('FastCaloEMReco', InViewRoIs='EMCaloRoIs')
    from TrigT2CaloEgamma.TrigT2CaloEgammaMTConfig import fastL2EgammaClusteringAlg
    ca = ComponentAccumulator()
    ca.addEventAlgo( CompFactory.AthViews.ViewDataVerifier(name='VDVFastEgammaCalo',
                                                           DataObjects = [('TrigRoiDescriptorCollection', 'StoreGateSvc+EMCaloRoIs'),
                                                                          ('CaloBCIDAverage', 'StoreGateSvc+CaloBCIDAverage') ]) )         
    ca.merge( fastL2EgammaClusteringAlg( flags, roisKey='EMCaloRoIs', doRinger=True) )
    reco.mergeReco( ca )
    return reco


