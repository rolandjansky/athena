# 
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
#

from AthenaCommon.AppMgr import ServiceMgr

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags


ServiceMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection = False

TrackParticlesName = "HLT_xAODTracks_Muon"
  
def dimuL2Sequence(name = 'Dimu'):

    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muNotCombAlgSequence
    (l2muNotCombSequence, l2muNotCombViewsMaker) = RecoFragmentsPool.retrieve(muNotCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigBphysHypo.TrigMultiTrkHypoMTConfig import TrigMultiTrkHypoMT
    from TrigBphysHypo.TrigMultiTrkHypoMTMonitoringConfig import TrigMultiTrkHypoAlgMTMonitoring
    
    jpsiHypo = TrigMultiTrkHypoMT(name+"HypoAlg")
    jpsiHypo.trackCollectionKey = TrackParticlesName
    jpsiHypo.bphysCollectionKey = "TrigBphys"+name
    jpsiHypo.nTrackMassMin = [100]
    jpsiHypo.nTrackMassMax = [20000]
    jpsiHypo.nTrk = 2
    jpsiHypo.MonTool = TrigMultiTrkHypoAlgMTMonitoring("TrigMultiTrkHypoAlgMTMonitoring_"+name)

    from TrigBphysHypo.TrigMultiTrkHypoMTConfig import TrigMultiTrkHypoToolMTFromDict

    return MenuSequence( Sequence    = l2muNotCombSequence,
                         Maker       = l2muNotCombViewsMaker,
                         Hypo        = jpsiHypo,
                         HypoToolGen = TrigMultiTrkHypoToolMTFromDict )