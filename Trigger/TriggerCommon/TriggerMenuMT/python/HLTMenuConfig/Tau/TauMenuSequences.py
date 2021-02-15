#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool

from TriggerMenuMT.HLTMenuConfig.Tau.TauRecoSequences import tauCaloSequence, tauCaloMVASequence, tauFTFCoreSequence, tauFTFTauSequence, tauFTFIsoSequence, tauFTFIsoBDTSequence, tauIDSequence, tauTrackSequence, tauTrackTwoSequence, tauEFSequence, tauMVASequence, tauPreSelSequence, tauPreSelTTSequence, tauPrecTrackSequence, tauPrecIsoTrackSequence

# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 

"""def getTauSequence( step ):
    if step == "calo":
        return tauCaloMenuSequence("Tau")
    if step == "calo_mva":
        return tauCaloMVAMenuSequence("Tau")
    if step == "track_twostep_core":
        return tauTwoStepTrackSeqCore()
    if step == "track_twostep_iso":
        return tauTwoStepTrackSeqIso()
    return None"""

# ===============================================================================================
#      Calo step
# ===============================================================================================

def tauCaloMenuSeq(name):
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

def tauCaloMVAMenuSeq(name):
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
#    Fast track finder (core) + TrackRoI Updater + RejectEmpty Hypo step (tracktwo, tracktwoEF, tracktwoMVA)                                             
# ===============================================================================================                                                   

def tauFTFTauCoreSeq():
    (sequence, ftfCoreViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFCoreSequence,ConfigFlags)

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo                 = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg_RejectEmpty")
    fastTrkHypo.trackcollection = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfCoreViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )


# ===============================================================================================                                                     
#    Fast track finder (tau) + Dummy Hypo step (ptOnly, track)                                                                            
# ===============================================================================================                                                          
  
def tauFTFTauSeq():
    (sequence, ftfTauViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFTauSequence,ConfigFlags)

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo                           = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg_PassByTau")
    fastTrkHypo.trackcollection           = sequenceOut
    fastTrkHypo.RoIForIDReadHandleKey     = ""
    #fastTrkHypo.AcceptAll       = True

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfTauViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )

# ===============================================================================================                                                           
#   Fast track finder (iso) + Dummy Hypo step (tracktwo, tracktwoEF, tracktwoMVA)                                                     
# ===============================================================================================                                                            

def tauFTFTauIsoSeq():
    (sequence, ftfIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIsoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo                 = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg_PassByIso")
    fastTrkHypo.trackcollection = sequenceOut
    #fastTrkHypo.AcceptAll       = True

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIsoViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )

# ===============================================================================================                                                                                                  
#   Fast track finder (iso bdt) + Dummy Hypo step (tracktwoMVABDT)                                                                                                                                 
# ===============================================================================================                                                                                                  

def tauFTFTauIsoBDTSeq():
    (sequence, ftfIsoBDTViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIsoBDTSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo                 = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg_PassByIsoBDT")
    fastTrkHypo.trackcollection = sequenceOut
    fastTrkHypo.RoIForIDReadHandleKey = "UpdatedTrackBDTRoI"

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIsoBDTViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )


# ===============================================================================================
#     Precision tracking + Tau Precision Alg + EFMVHypo step   (pTonly)
# ===============================================================================================

def tauIDPrecSeq():
    (sequence, tauIDViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauIDSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoId")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauIDViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )


# ===============================================================================================                                
#     Tau Preselection Alg + Precision Tracking + Tau Precision Alg + EFMVHypo step  (track)                                                                 
# ===============================================================================================

def tauTrackPrecSeq():
    (sequence, tauTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoTrack")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauTrackViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                            
#     Tau Preselection Alg + Precision Tracking + Tau Precision Alg + EFMVHypo step   (tracktwo)
# ===============================================================================================                                              

def tauTrackTwoPrecSeq():
    (sequence, trackTwoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauTrackTwoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoTrackTwo")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = trackTwoViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                             
#     Precision Tracking + Tau Precision Alg + EFMVHypo step   (tracktwoEF)
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

# ===============================================================================================
#     Precision Tracking + Tau Precision MVA Alg + EFMVHypo step   (tracktwoMVA)
# ===============================================================================================

def tauTrackTwoMVASeq():
    (sequence, mvaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauMVASequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoAlgMVA")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = mvaViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                                            
#     Tau Preselection + EFMVHypo step   (track, tracktwo)                                                                             
# =============================================================================================== 


def tauPreSelSeq():
    (sequence, preSelViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPreSelSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    preSelHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoAlgPreSel")
    preSelHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = preSelViewsMaker,
                          Hypo        = preSelHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                                            
#     Tau Preselection + EFMVHypo step   (track, tracktwo)                                                                                                   
# ===============================================================================================                                                            


def tauPreSelTTSeq():
    (sequence, preSelViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPreSelTTSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    preSelHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoAlgPreSelTT")
    preSelHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = preSelViewsMaker,
                          Hypo        = preSelHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                                            
#     Precision Tracking + TrkPrecHypo step   (tracktwoMVA)                                                                             
# =============================================================================================== 

def tauPrecTrackSeq():
    (sequence, precTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPrecTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrkPrecHypoAlgMT
    precTrkHypo = TrigTrkPrecHypoAlgMT("TrkPrecHypoAlg")
    precTrkHypo.trackparticles        = sequenceOut
    precTrkHypo.RoIForIDReadHandleKey = ""

    from TrigTauHypo.TrigTauHypoTool import TrigTrkPrecHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = precTrackViewsMaker,
                          Hypo        = precTrkHypo,
                          HypoToolGen = TrigTrkPrecHypoToolFromDict )

# ===============================================================================================                                                            
#     Precision Tracking + TrkPrecHypo step   (tracktwoMVA)                                                                                                  
# ===============================================================================================                                                             

def tauPrecTrackIsoSeq():
    (sequence, precTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPrecIsoTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrkPrecHypoAlgMT
    precTrkHypo = TrigTrkPrecHypoAlgMT("TrkPrecIsoHypoAlg")
    precTrkHypo.trackparticles        = sequenceOut
    precTrkHypo.RoIForIDReadHandleKey = ""

    from TrigTauHypo.TrigTauHypoTool import TrigTrkPrecHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = precTrackViewsMaker,
                          Hypo        = precTrkHypo,
                          HypoToolGen = TrigTrkPrecHypoToolFromDict )
