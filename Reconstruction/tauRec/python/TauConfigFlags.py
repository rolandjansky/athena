# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
import AthenaCommon.SystemOfUnits as Units

def createTauConfigFlags(): 
    tau_cfg=AthConfigFlags()
 
    tau_cfg.addFlag("Tau.doTauRec",True)

    # Output containers
    tau_cfg.addFlag("Tau.outputType","xAOD::TauJetContainer")
    tau_cfg.addFlag("Tau.outputKey","TauJets")

    # Input containers
    tau_cfg.addFlag("Tau.VertexCollection","PrimaryVertices")
    tau_cfg.addFlag("Tau.TrackCollection","InDetTrackParticles")
    tau_cfg.addFlag("Tau.SeedJetCollection", "AntiKt4LCTopoJets")
    tau_cfg.addFlag("Tau.LargeD0TrackCollection","InDetLargeD0TrackParticles")

    # Switches for enabling/disabling some tools
    tau_cfg.addFlag("Tau.doTJVA",True)
    tau_cfg.addFlag("Tau.doPi0Clus",True)
    tau_cfg.addFlag("Tau.doPanTau",True)
    tau_cfg.addFlag("Tau.doRNNTrackClass",True)
    tau_cfg.addFlag("Tau.doTauDiscriminant",True)
    tau_cfg.addFlag("Tau.associateLRT",False)
    tau_cfg.addFlag("Tau.isStandalone",False)
    # Classify Large Radius Tracks in tau track classifier
    tau_cfg.addFlag("Tau.clasifyLRT",False)

    # Settings common to Run2 and Run3
    tau_cfg.addFlag("Tau.SeedMinPt", 0.0*Units.GeV)
    tau_cfg.addFlag("Tau.SeedMaxEta", 2.5)
    # FIXME: MaxNTracks is not used, drop at the next occasion
    tau_cfg.addFlag("Tau.MaxNTracks", -1)
    tau_cfg.addFlag("Tau.RemoveDupeCoreTracks",True)
    tau_cfg.addFlag("Tau.doTJVATiebreak",True)
    tau_cfg.addFlag("Tau.useGhostTracks",True)
    tau_cfg.addFlag("Tau.ghostTrackDR", 0.25)
    tau_cfg.addFlag("Tau.shotPtCut_1Photon", [430.*Units.MeV,300.*Units.MeV,9999999.*Units.MeV,330.*Units.MeV,350.*Units.MeV])
    tau_cfg.addFlag("Tau.shotPtCut_2Photons", [10000.*Units.MeV,10000.*Units.MeV,9999999.*Units.MeV,10000.*Units.MeV,10000.*Units.MeV])
    tau_cfg.addFlag("Tau.Pi0ScoreConfig", "TauPi0BDTWeights.root")
    tau_cfg.addFlag("Tau.pi0EtCuts",[2800.*Units.MeV,2700.*Units.MeV,2500.*Units.MeV,2900.*Units.MeV,2700.*Units.MeV])
    tau_cfg.addFlag("Tau.pi0MVACuts_1prong",[0.45,0.37,0.39,0.40,0.38])
    tau_cfg.addFlag("Tau.pi0MVACuts_mprong",[0.73,0.69,0.58,0.69,0.67])

    # Run2 settings and calibration files
    tau_cfg.addFlag("Tau.tauRecToolsCVMFSPath","tauRecTools/R22_preprod")
    tau_cfg.addFlag("Tau.tauRNNTrackClassConfig","RNNTrackClassifier_2021-07-19_14-25-14_90_25_30.json")
    tau_cfg.addFlag("Tau.CalibrateLCConfig", "CaloTES_R22_v1.root")
    tau_cfg.addFlag("Tau.CombinedTESConfig", "CombinedTES_R22_Round2.5.root")
    tau_cfg.addFlag("Tau.MvaTESConfig0p", "MvaTES_0p_R22_Round3.root")
    tau_cfg.addFlag("Tau.MvaTESConfig", "MvaTES_R22_Round3.root")
    tau_cfg.addFlag("Tau.MinPt0p", 9.25*Units.GeV)
    tau_cfg.addFlag("Tau.MinPt", 6.75*Units.GeV)
    tau_cfg.addFlag("Tau.TauJetRNNConfig", ["tauid_rnn_1p_R22_v1.json", "tauid_rnn_2p_R22_v1.json", "tauid_rnn_3p_R22_v1.json"])
    tau_cfg.addFlag("Tau.TauJetRNNWPConfig", ["tauid_rnnWP_1p_R22_v0.root", "tauid_rnnWP_2p_R22_v0.root", "tauid_rnnWP_3p_R22_v0.root"])
    tau_cfg.addFlag("Tau.TauEleRNNConfig", ["taueveto_rnn_config_1P_r22.json", "taueveto_rnn_config_3P_r22.json"])
    tau_cfg.addFlag("Tau.TauEleRNNWPConfig", ["taueveto_rnn_flat_1P_r22.root", "taueveto_rnn_flat_3P_r22.root"])
    tau_cfg.addFlag("Tau.DecayModeNNClassifierConfig", "NNDecayMode_R22_v1.json")

    return tau_cfg

# Self test
import unittest

class TestTauRecConfigFlags(unittest.TestCase):
    def runTest(self):
        createTauConfigFlags()
        
if __name__ == "__main__":
    unittest.main()
