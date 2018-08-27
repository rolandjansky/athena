#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def l2PhotonCaloStep( flags, chains ):

    from AthenaCommon.Constants import DEBUG
    acc = ComponentAccumulator()

    # setup algorithms
    #acc.addSequence( seqAND('L2CaloEgamma'), parentName=parentSeq )
    from TrigUpgradeTest.MenuComponents import FilterHypoSequence
    fhSeq = FilterHypoSequence( 'PhotonFastCalo' )
    fhSeq.addFilter( chains, inKey = 'EMRoIDecisions', outKey='FilteredPhotonRoIDecisions' )

    from TrigUpgradeTest.ElectronMenuConfig import l2CaloReco # generator for the L2 Calo EM clustering
    from TrigUpgradeTest.MenuComponents import RecoFragmentsPool 

    # obtain the reconstruction CF fragment    
    fhSeq.addReco( RecoFragmentsPool.retrieve( l2CaloReco, flags ) )

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    hypo = TrigL2CaloHypoAlgMT("L2PhotonCaloHypo")
    hypo.CaloClusters        = 'L2CaloClusters'
    hypo.HypoOutputDecisions = 'PhotonFastCaloDecisions'
    hypo.OutputLevel = DEBUG
    

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName    
    for chain in chains:
        tool  = TrigL2CaloHypoToolFromName( chain, chain )
        hypo.HypoTools +=  [ tool ]
        
    fhSeq.addHypo( hypo )
    
    return fhSeq


def generatePhotons( flags ):

    acc = ComponentAccumulator()
    photonChains = [ f.split()[0] for f in flags.get("Trigger.menu.photons") ]
    if not photonChains:
        return None,None
    l2CaloSequence = l2PhotonCaloStep( flags, photonChains )
    acc.merge( l2CaloSequence )


    return acc, [l2CaloSequence]
