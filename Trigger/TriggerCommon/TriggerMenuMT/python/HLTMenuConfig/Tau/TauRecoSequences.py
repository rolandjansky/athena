#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool


def _algoTauRoiUpdater(inputRoIs, clusters):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdaterMT
    algo = TrigTauCaloRoiUpdaterMT("TauCaloRoiUpdater")
    algo.RoIInputKey  = inputRoIs
    algo.RoIOutputKey = "RoiForTau"
    algo.CaloClustersKey = clusters
    return algo

def _algoTauCaloOnly(inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnly
    roiUpdateAlgo = _algoTauRoiUpdater(inputRoIs, clusters)
    algo = TrigTauRecMerged_TauCaloOnly()
    algo.RoIInputKey         = inputRoIs
    algo.clustersKey         = clusters
    algo.L1RoIKey            = roiUpdateAlgo.RoIInputKey
    algo.TrigTauRecOutputKey = recordable("HLT_TrigTauRecMerged")
    return algo

def _algoTauCaloOnlyMVA(inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnlyMVA
    roiUpdateAlgo = _algoTauRoiUpdater(inputRoIs, clusters)
    algo = TrigTauRecMerged_TauCaloOnlyMVA()
    algo.RoIInputKey         = inputRoIs
    algo.L1RoIKey            = roiUpdateAlgo.RoIInputKey
    algo.clustersKey         = clusters
    algo.TrigTauRecOutputKey = recordable("HLT_TrigTauRecMerged")
    return algo

def tauCaloRecoSequence(InViewRoIs, SeqName):
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, InViewRoIs)
    tauCaloRoiUpdaterAlg = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    tauCaloOnlyAlg       = _algoTauCaloOnly(inputRoIs   = InViewRoIs, clusters = lcCaloSequenceOut)
    RecoSequence = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyAlg] )
    return (RecoSequence, tauCaloOnlyAlg.TrigTauRecOutputKey)

def tauCaloMVARecoSequence(InViewRoIs, SeqName):
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, InViewRoIs)
    tauCaloRoiUpdaterAlg = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    tauCaloOnlyMVAAlg	 = _algoTauCaloOnlyMVA(inputRoIs   = InViewRoIs, clusters = lcCaloSequenceOut)
    RecoSequence = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyMVAAlg] )
    return (RecoSequence, tauCaloOnlyMVAAlg.TrigTauRecOutputKey)

def tauCaloSequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs="TAUCaloRoIs"
    RecoSequenceName="tauCaloInViewSequence"

    tauCaloViewsMaker = EventViewCreatorAlgorithm( "tauCaloViewsMaker")
    tauCaloViewsMaker.ViewFallThrough = True
    tauCaloViewsMaker.RoIsLink = "initialRoI"
    tauCaloViewsMaker.InViewRoIs = InViewRoIs
    tauCaloViewsMaker.Views = "TAUCaloViews"
    tauCaloViewsMaker.ViewNodeName = RecoSequenceName
    (tauCaloInViewSequence, sequenceOut) = tauCaloRecoSequence( InViewRoIs, RecoSequenceName)

    tauCaloSequence = seqAND("tauCaloSequence", [tauCaloViewsMaker, tauCaloInViewSequence ])
    return (tauCaloSequence, tauCaloViewsMaker, sequenceOut)    

def tauCaloMVASequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs="TAUCaloRoIs"
    RecoSequenceName="tauCaloMVAInViewSequence"

    tauCaloMVAViewsMaker = EventViewCreatorAlgorithm( "tauCaloMVAViewsMaker")
    tauCaloMVAViewsMaker.ViewFallThrough = True
    tauCaloMVAViewsMaker.RoIsLink = "initialRoI"
    tauCaloMVAViewsMaker.InViewRoIs = InViewRoIs
    tauCaloMVAViewsMaker.Views = "TAUCaloMVAViews"
    tauCaloMVAViewsMaker.ViewNodeName = RecoSequenceName
    (tauCaloMVAInViewSequence, sequenceOut) = tauCaloMVARecoSequence(InViewRoIs, RecoSequenceName)

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)
