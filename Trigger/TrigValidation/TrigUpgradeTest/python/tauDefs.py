#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO, ERROR
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigT2CaloCommon.CaloDef import HLTCellMaker, HLTLCTopoRecoSequence

def _algoTauRoiUpdater(inputEDM="EMRoIs"):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdaterMT
    algo = TrigTauCaloRoiUpdaterMT("CaloRoiUpdater")
    algo.RoIInputKey  = "TAUCaloRoIs"
    algo.RoIOutputKey = "RoiForTau"
    return algo

def _algoTauCaloOnly(inputEDM="EMRoIs"):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnly
    roiUpdateAlgo = _algoTauRoiUpdater(inputEDM)
    algo = TrigTauRecMerged_TauCaloOnly()
    algo.RoIInputKey         = inputEDM
    algo.L1RoIKey            = roiUpdateAlgo.RoIInputKey
    algo.clustersKey         = "caloclusters"
    algo.TrigTauRecOutputKey = "taujets"
    return algo

def tauCaloRecoSequence(InViewRoIs):
    # lc sequence
    (lcTopoInViewSequence, sequenceOut) = HLTLCTopoRecoSequence(InViewRoIs)
    tauCaloRoiUpdaterAlg = _algoTauRoiUpdater(inputEDM=InViewRoIs)
    tauCaloOnlyAlg       = _algoTauCaloOnly(inputEDM=InViewRoIs)
    RecoSequence = parOR( 'tauCaloInViewSequence', [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyAlg] )
    sequenceOut = tauCaloOnlyAlg.TrigTauRecOutputKey
    return (RecoSequence, sequenceOut)

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
