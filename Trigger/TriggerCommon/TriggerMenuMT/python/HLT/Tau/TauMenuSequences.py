#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, RecoFragmentsPool

from TriggerMenuMT.HLT.Tau.TauRecoSequences import tauCaloMVASequence, tauFTFCoreSequence, tauFTFIsoSequence, tauFTFIsoBDTSequence, tauMVASequence, tauLLPSequence, tauPrecIsoTrackSequence

# ===============================================================================================
#      Calo MVA step
# ===============================================================================================

def tauCaloMVAMenuSeq(name, is_probe_leg=False):
    (sequence, tauCaloMVAViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauCaloMVASequence,ConfigFlags)

    # hypo
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloHypoAlg
    theTauCaloMVAHypo = TrigTauCaloHypoAlg(name+"L2CaloMVAHypo")
    theTauCaloMVAHypo.taujets     = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigL2TauHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauCaloMVAViewsMaker,
                          Hypo        = theTauCaloMVAHypo,
                          HypoToolGen = TrigL2TauHypoToolFromDict,
                          IsProbe     = is_probe_leg )

# ===============================================================================================                                
#    Fast track finder (core) + TrackRoI Updater + RejectEmpty Hypo step (tracktwoMVA)                                             
# ===============================================================================================                                                   

def tauFTFTauCoreSeq(is_probe_leg=False):
    (sequence, ftfCoreViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFCoreSequence,ConfigFlags)

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlg
    fastTrkHypo                 = TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_RejectEmpty")
    fastTrkHypo.trackcollection = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfCoreViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict,
                          IsProbe     = is_probe_leg )

# ===============================================================================================                                                           
#   Fast track finder (iso) + Dummy Hypo step (tracktwoMVA)                                                     
# ===============================================================================================                                                            

def tauFTFTauIsoSeq(is_probe_leg=False):
    (sequence, ftfIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIsoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlg
    fastTrkHypo                 = TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIso")
    fastTrkHypo.trackcollection = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIsoViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict,
                          IsProbe     = is_probe_leg )

# ===============================================================================================                                                            
#   Fast track finder (iso bdt) + Dummy Hypo step (tracktwoMVABDT)                                                                                           
# ===============================================================================================                                 

def tauFTFTauIsoBDTSeq(is_probe_leg=False):
    (sequence, ftfIsoBDTViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIsoBDTSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlg
    fastTrkHypo                 = TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIsoBDT")
    fastTrkHypo.trackcollection = sequenceOut
    fastTrkHypo.RoIForIDReadHandleKey = "UpdatedTrackBDTRoI"

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIsoBDTViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict,
                          IsProbe     = is_probe_leg )

# ===============================================================================================
#     Tau Precision MVA Alg + EFMVHypo step   (tracktwoMVA)
# ===============================================================================================

def tauTrackTwoMVASeq(is_probe_leg=False):
    (sequence, mvaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauMVASequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    precisionHypo = TrigEFTauMVHypoAlg("EFTauMVHypoAlgMVA")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = mvaViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict,
                          IsProbe     = is_probe_leg )

# ===============================================================================================
#     Tau Precision LLP Alg + EFMVHypo step   (tracktwoLLP)
# ===============================================================================================

def tauTrackTwoLLPSeq(is_probe_leg=False):
    (sequence, mvaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauLLPSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    precisionHypo = TrigEFTauMVHypoAlg("EFTauMVHypoAlgLLP")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = mvaViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict,
                          IsProbe     = is_probe_leg )

# ===============================================================================================                                                            
#     Precision Tracking + TrkPrecHypo step   (tracktwoEF, tracktwoMVA, tracktwoMVABDT)                                                                                               
# ===============================================================================================                                                           

def tauPrecTrackIsoSeq(is_probe_leg=False):
    (sequence, precTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPrecIsoTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrkPrecHypoAlg
    precTrkHypo = TrigTrkPrecHypoAlg("TrkPrecIsoHypoAlg")
    precTrkHypo.trackparticles        = sequenceOut
    precTrkHypo.RoIForIDReadHandleKey = ""

    from TrigTauHypo.TrigTauHypoTool import TrigTrkPrecHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = precTrackViewsMaker,
                          Hypo        = precTrkHypo,
                          HypoToolGen = TrigTrkPrecHypoToolFromDict,
                          IsProbe     = is_probe_leg )
