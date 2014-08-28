# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
import traceback
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
from AthenaCommon.Constants import INFO,WARNING,DEBUG,VERBOSE,ERROR

def TruthTools(MCParticleContainer,
               MCEventCollection,
               PAUdoESD,
               TruthJetsCollection,
               exToCalo):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr

    useSeparateTruthClassifier = True
    if useSeparateTruthClassifier:
        import MCTruthClassifier.MCTruthClassifierBase
        from  MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
        mymctruth = MCTruthClassifier("PAUMCTruthClassifier")
        mymctruth.McEventCollection = MCEventCollection
        mymctruth.ExtrapolateToCaloTool = exToCalo
        mymctruth.pTChargePartCut  = 1    # BEWARE: must be in GeV(!!!)
        mymctruth.pTNeutralPartCut = 0.5  # BEWARE: must be in GeV(!!!)
        mymctruth.inclG4part = True
        mymctruth.ROICone = False
        mymctruth.OutputLevel = ERROR
        try:
            mymctruth.forceNotUseBremRefitTrk = False
        except AttributeError:
            pass
        ToolSvc += mymctruth
    else:
        from TruthD3PDMaker.MCTruthClassifierConfig import D3PDMCTruthClassifier
        mymctruth = D3PDMCTruthClassifier
        D3PDMCTruthClassifier.OutputLevel = WARNING

    from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthTool
    ToolSvc += PAUtruthTool()
    # --- Truth handling:
    # ToolSvc.PAUtruthTool.EGammasFromTruth = True # all e/gamma are kept (otherwise only gammas from H->gg) (default is true)
    # ToolSvc.PAUtruthTool.MuonsFromTruth = True   # all muons are kept (default is true)
    # ToolSvc.PAUtruthTool.HadronsFromTruth = True # all hadrons are kept (default is true)
    # ToolSvc.PAUtruthTool.PartonsFromTruth = True # all partons are kept (default is false)
    # ToolSvc.PAUtruthTool.RTruthConv = ...  # maximum conversion/interaction radius (in mm, default is 800)
    # ToolSvc.PAUtruthTool.ZTruthConv = ...  # maximum conversion/interaction z (in mm, default is 2800)
    # ToolSvc.PAUtruthTool.PtTruthCut = ...  # reject truth particles produced with pt<PtTruthCut (in MeV)
    # ToolSvc.PAUtruthTool.deltaRmatch = ... # deltaR for angular Reco/Truth match (default is 0.1)
    # --- Containers' SG keys
    ToolSvc.PAUtruthTool.MCParticleContainer = MCParticleContainer
    ToolSvc.PAUtruthTool.MCEventCollection = MCEventCollection
    ToolSvc.PAUtruthTool.ConvertFromESD = PAUdoESD
    ToolSvc.PAUtruthTool.TruthJetsContainer = TruthJetsCollection
    # --- NEW truth match settings:
    ToolSvc.PAUtruthTool.PtTruthCut = 500
    ToolSvc.PAUtruthTool.UseExtrapolation = True
    ToolSvc.PAUtruthTool.UseG4Particles = True
    ToolSvc.PAUtruthTool.deltaEtaROI = 9999
    ToolSvc.PAUtruthTool.deltaPhiROI = 9999
    ToolSvc.PAUtruthTool.PtCutCharged = 1000
    ToolSvc.PAUtruthTool.PtCutNeutral =  500
    # --- end NEW truth match settings:
    # --- new MCTruthClassifier settings:
    # ## uncomment to use PAU truth match for photons:
    # ToolSvc.PAUtruthTool.UseTruthClassifierTool = False
    # ##
    ToolSvc.PAUtruthTool.TruthClassifierTool = mymctruth
    # --- end MCTruthClassifier settings

    if PAUdoESD:
        from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
        truthParticleCnvTool = TruthParticleCnvTool()
        truthParticleCnvTool.McEvents = MCEventCollection
        truthParticleCnvTool.TruthParticlesOutput = MCParticleContainer
        ToolSvc.PAUtruthTool.CnvTool = truthParticleCnvTool
        
    from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthMatchingTool
    ToolSvc += PAUtruthMatchingTool()
    ToolSvc.PAUtruthMatchingTool.OutputLevel = INFO

    from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthTrackClassifier
    ToolSvc += PAUtruthTrackClassifier()
    ToolSvc.PAUtruthTrackClassifier.EgammaTruthClassifierTool = mymctruth


    return ToolSvc.PAUtruthTool


def TruthToolsWrapper(exToCalo=None):

    from RecExConfig.RecFlags  import rec
    if rec.readESD():
        MCEventCollection = "TruthEvent"
    else:
        MCEventCollection = "GEN_AOD"
        
    return TruthTools(MCParticleContainer = "SpclMC",
                      MCEventCollection   = MCEventCollection,
                      PAUdoESD            = rec.readESD(),
                      TruthJetsCollection = "AntiKt4TruthJets",
                      exToCalo            = exToCalo)

