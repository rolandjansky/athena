#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable

def _algoTauRoiUpdater(inputRoIs="EMRoIs"):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdaterMT
    algo = TrigTauCaloRoiUpdaterMT("CaloRoiUpdater")
    algo.RoIInputKey  = "TAUCaloRoIs"
    algo.RoIOutputKey = "RoiForTau"
    return algo

def _algoTauCaloOnly(inputRoIs="EMRoIs"):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnly
    roiUpdateAlgo = _algoTauRoiUpdater(inputRoIs)
    algo = TrigTauRecMerged_TauCaloOnly()
    algo.RoIInputKey         = inputRoIs
    algo.L1RoIKey            = roiUpdateAlgo.RoIInputKey
    algo.clustersKey         = "caloclusters"
    algo.TrigTauRecOutputKey = recordable("HLT_TrigTauRecMerged")
    return algo

def tauCaloRecoSequence(InViewRoIs):
    # lc sequence
    (lcTopoInViewSequence, sequenceOut) = HLTLCTopoRecoSequence(InViewRoIs)
    tauCaloRoiUpdaterAlg = _algoTauRoiUpdater(inputRoIs = InViewRoIs)
    tauCaloOnlyAlg       = _algoTauCaloOnly(inputRoIs   = InViewRoIs)
    RecoSequence = parOR( 'tauCaloInViewSequence', [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyAlg] )
    return (RecoSequence, tauCaloOnlyAlg.TrigTauRecOutputKey)

def tauCaloSequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs="TAUCaloRoIs"
    tauCaloViewsMaker = EventViewCreatorAlgorithm( "tauCaloViewsMaker")
    tauCaloViewsMaker.ViewFallThrough = True
    tauCaloViewsMaker.RoIsLink = "initialRoI"
    tauCaloViewsMaker.InViewRoIs = InViewRoIs
    tauCaloViewsMaker.Views = "TAUCaloViews"
    tauCaloViewsMaker.ViewNodeName = "tauCaloInViewSequence"

    (tauCaloInViewSequence, sequenceOut) = tauCaloRecoSequence(InViewRoIs)

    tauCaloSequence = seqAND("tauCaloSequence", [tauCaloViewsMaker, tauCaloInViewSequence ])
    return (tauCaloSequence, tauCaloViewsMaker, sequenceOut)
