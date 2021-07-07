#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool

from TriggerMenuMT.HLTMenuConfig.Tau.TauRecoSequences import tauCaloSequence, tauCaloMVASequence, tauFTFCoreSequence, tauFTFTauSequence, tauFTFIsoSequence, tauFTFIsoBDTSequence, tauTrackSequence, tauTrackTwoSequence, tauMVASequence, tauLLPSequence, tauPreSelSequence, tauPreSelTTSequence, tauPrecTrackSequence, tauPrecIsoTrackSequence

# ===============================================================================================
#      Calo step
# ===============================================================================================

def tauCaloMenuSeq(name):
    (sequence, tauCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauCaloSequence,ConfigFlags)

    # hypo
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloHypoAlg
    theTauCaloHypo = TrigTauCaloHypoAlg(name+"L2CaloHypo")
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
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloHypoAlg
    theTauCaloMVAHypo = TrigTauCaloHypoAlg(name+"L2CaloMVAHypo")
    theTauCaloMVAHypo.taujets     = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigL2TauHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauCaloMVAViewsMaker,
                          Hypo        = theTauCaloMVAHypo,
                          HypoToolGen = TrigL2TauHypoToolFromDict )

# ===============================================================================================                                
#    Fast track finder (core) + TrackRoI Updater + RejectEmpty Hypo step (tracktwo, tracktwoMVA)                                             
# ===============================================================================================                                                   

def tauFTFTauCoreSeq():
    (sequence, ftfCoreViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFCoreSequence,ConfigFlags)

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlg
    fastTrkHypo                 = TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_RejectEmpty")
    fastTrkHypo.trackcollection = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfCoreViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )


# ===============================================================================================                                                     
#    Fast track finder (tau) + Dummy Hypo step (track)                                                                            
# ===============================================================================================                                                          
  
def tauFTFTauSeq():
    (sequence, ftfTauViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFTauSequence,ConfigFlags)

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlg
    fastTrkHypo                           = TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByTau")
    fastTrkHypo.trackcollection           = sequenceOut
    fastTrkHypo.RoIForIDReadHandleKey     = ""

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    TrigTrackPreSelHypoTool.AcceptAll = True

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfTauViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )

# ===============================================================================================                                                           
#   Fast track finder (iso) + Dummy Hypo step (tracktwo, tracktwoMVA)                                                     
# ===============================================================================================                                                            

def tauFTFTauIsoSeq():
    (sequence, ftfIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIsoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlg
    fastTrkHypo                 = TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIso")
    fastTrkHypo.trackcollection = sequenceOut

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    TrigTrackPreSelHypoTool.AcceptAll = True

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

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlg
    fastTrkHypo                 = TrigTrackPreSelHypoAlg("TrackPreSelHypoAlg_PassByIsoBDT")
    fastTrkHypo.trackcollection = sequenceOut
    fastTrkHypo.RoIForIDReadHandleKey = "UpdatedTrackBDTRoI"

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    TrigTrackPreSelHypoTool.AcceptAll = True

    from TrigTauHypo.TrigTauHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIsoBDTViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )


# ===============================================================================================                                
#     Tau Precision Alg + EFMVHypo step  (track)                                                                 
# ===============================================================================================

def tauTrackPrecSeq():
    (sequence, tauTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    precisionHypo = TrigEFTauMVHypoAlg("EFTauMVHypoTrack")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauTrackViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                            
#     Tau Precision Alg + EFMVHypo step   (tracktwo)
# ===============================================================================================                                              

def tauTrackTwoPrecSeq():
    (sequence, trackTwoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauTrackTwoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    precisionHypo = TrigEFTauMVHypoAlg("EFTauMVHypoTrackTwo")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = trackTwoViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================
#     Tau Precision MVA Alg + EFMVHypo step   (tracktwoMVA)
# ===============================================================================================

def tauTrackTwoMVASeq():
    (sequence, mvaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauMVASequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    precisionHypo = TrigEFTauMVHypoAlg("EFTauMVHypoAlgMVA")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = mvaViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================                                                                                                                                     
#     Tau Precision MVA Alg + EFMVHypo test step   (tracktwoMVATest)                                                                                                                                                                  
# ===============================================================================================                                                                                                                                     

def tauTrackTwoMVATestSeq():
    (sequence, mvaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauMVASequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    precisionHypo = TrigEFTauMVHypoAlg("EFTauMVHypoAlgMVATest")
    precisionHypo.taujetcontainer = sequenceOut

    #Turn off high-pt ID loose cut loosening for testing purposes
    from TrigTauHypo.TrigTauHypoConf import TrigEFTauMVHypoTool
    TrigEFTauMVHypoTool.highpt = False

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = mvaViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )

# ===============================================================================================
#     Tau Precision LLP Alg + EFMVHypo step   (tracktwoLLP)
# ===============================================================================================

def tauTrackTwoLLPSeq():
    (sequence, mvaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauLLPSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    precisionHypo = TrigEFTauMVHypoAlg("EFTauMVHypoAlgLLP")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = mvaViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )


# ===============================================================================================                                                            
#     Tau Preselection + EFMVHypo step   (track)                                                                             
# =============================================================================================== 

def tauPreSelSeq():
    (sequence, preSelViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPreSelSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    preSelHypo = TrigEFTauMVHypoAlg("EFTauMVHypoAlgPreSel")
    preSelHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    TrigTrackPreSelHypoTool.AcceptAll = True

    from TrigTauHypo.TrigTauHypoTool import TrigPresTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = preSelViewsMaker,
                          Hypo        = preSelHypo,
                          HypoToolGen = TrigPresTauMVHypoToolFromDict )

# ===============================================================================================                                                            
#     Tau Preselection + EFMVHypo step   (tracktwo)                                                                                                   
# ===============================================================================================                                                            

def tauPreSelTTSeq():
    (sequence, preSelViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPreSelTTSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlg
    preSelHypo = TrigEFTauMVHypoAlg("EFTauMVHypoAlgPreSelTT")
    preSelHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    TrigTrackPreSelHypoTool.AcceptAll = True

    from TrigTauHypo.TrigTauHypoTool import TrigPresTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = preSelViewsMaker,
                          Hypo        = preSelHypo,
                          HypoToolGen = TrigPresTauMVHypoToolFromDict )

# ===============================================================================================                                                            
#     Precision Tracking + TrkPrecHypo step   (track)                                                                             
# =============================================================================================== 

def tauPrecTrackSeq():
    (sequence, precTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPrecTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrkPrecHypoAlg
    precTrkHypo = TrigTrkPrecHypoAlg("TrkPrecHypoAlg")
    precTrkHypo.trackparticles        = sequenceOut
    precTrkHypo.RoIForIDReadHandleKey = ""

    from TrigTauHypo.TrigTauHypoTool import TrigTrkPrecHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = precTrackViewsMaker,
                          Hypo        = precTrkHypo,
                          HypoToolGen = TrigTrkPrecHypoToolFromDict )

# ===============================================================================================                                                            
#     Precision Tracking + TrkPrecHypo step   (tracktwo, tracktwoEF, tracktwoMVA, tracktwoMVABDT)                                                                                               
# ===============================================================================================                                                           

def tauPrecTrackIsoSeq():
    (sequence, precTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauPrecIsoTrackSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrkPrecHypoAlg
    precTrkHypo = TrigTrkPrecHypoAlg("TrkPrecIsoHypoAlg")
    precTrkHypo.trackparticles        = sequenceOut
    precTrkHypo.RoIForIDReadHandleKey = ""

    from TrigTauHypo.TrigTauHypoTool import TrigTrkPrecHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = precTrackViewsMaker,
                          Hypo        = precTrkHypo,
                          HypoToolGen = TrigTrkPrecHypoToolFromDict )
