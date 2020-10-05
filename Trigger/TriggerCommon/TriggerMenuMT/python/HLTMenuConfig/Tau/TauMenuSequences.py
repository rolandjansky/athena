#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool

from TriggerMenuMT.HLTMenuConfig.Tau.TauRecoSequences import tauCaloSequence, tauCaloMVASequence, tauFTFCoreSequence, tauFTFIsoSequence, tauFTFIdSequence, tauFTFTrackSequence, tauFTFTrackTwoSequence, tauEFSequence

# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 

def getTauSequence( step ):
    if step == "calo":
        return tauCaloMenuSequence("Tau")
    if step == "calo_mva":
        return tauCaloMVAMenuSequence("Tau")
    if step == "track_twostep_core":
        return tauTwoStepTrackSeqCore()
    if step == "track_twostep_iso":
        return tauTwoStepTrackSeqIso()
    return None

# ===============================================================================================
#      Calo step
# ===============================================================================================

def tauCaloMenuSequence(name):
    (sequence, tauCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauCaloSequence,ConfigFlags)

    # hypo
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloHypoAlgMT
    theTauCaloHypo = TrigTauCaloHypoAlgMT(name+"L2CaloHypo")
    theTauCaloHypo.taujets     = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigL2TauHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauCaloViewsMaker,
                          Hypo        = theTauCaloHypo,
                          HypoToolGen = TrigL2TauHypoToolFromDict )

# ===============================================================================================
#      Calo MVA step
# ===============================================================================================

def tauCaloMVAMenuSequence(name):
    (sequence, tauCaloMVAViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauCaloMVASequence,ConfigFlags)

    # hypo
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloHypoAlgMT
    theTauCaloMVAHypo = TrigTauCaloHypoAlgMT(name+"L2CaloMVAHypo")
    theTauCaloMVAHypo.taujets     = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigL2TauHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauCaloMVAViewsMaker,
                          Hypo        = theTauCaloMVAHypo,
                          HypoToolGen = TrigL2TauHypoToolFromDict )

# ===============================================================================================                                
#     Fast track (ID trig) + precision tracking + EFMVHypo step                                                                  
# ===============================================================================================

def tauTrackSeq():

    (sequence, ftfTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoTrack")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfTrackViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                            
#     Fast track (ID trig) + precision tracking + EFMVHypo step                                                                               
# ===============================================================================================                                              

def tauTrackTwoSeq():

    (sequence, ftfTrackTwoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFTrackTwoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoTrackTwo")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfTrackTwoViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )


# ===============================================================================================
#     Fast track (ID trig) + precision tracking + EFMVHypo step   
# ===============================================================================================

def tauIdTrackSeq():

    (sequence, ftfIdViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIdSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoId")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIdViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )


# ===============================================================================================                                
#    Fast track core + RejectEmpty Hypo step (tracktwo, tracktwoEF, tracktwoMVA)                                                              
# ===============================================================================================                                                   

def tauTwoStepTrackSeqCore():

    (sequence, ftfCoreViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFCoreSequence,ConfigFlags)

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg_RejectEmpty")
    fastTrkHypo.trackcollection = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfCoreViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )

# ===============================================================================================
#                                                                                               
# ===============================================================================================


def tauTwoStepTrackSeqIso():

    (sequence, ftfIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIsoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoAlgFinal")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIsoViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )


# ===============================================================================================                                             
#                                                                                                                                             
# ===============================================================================================                                                                                


def tauTrackTwoEFSeq():

    (sequence, efViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauEFSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoAlg")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = efViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )
