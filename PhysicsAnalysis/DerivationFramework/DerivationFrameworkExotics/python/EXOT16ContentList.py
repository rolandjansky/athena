# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT16Content= {
"AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"    :   "xAOD::JetContainer"          ,
"AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux" :   "xAOD::JetAuxContainer"       ,
"AntiKtVR600Rmax10Rmin2LCTopoJets"            :   "xAOD::JetContainer"          ,
"AntiKtVR600Rmax10Rmin2LCTopoJetsAux"         :   "xAOD::JetAuxContainer"       ,
"AntiKtVR60Rmax4Rmin2LCTopoJets"              :   "xAOD::JetContainer"          ,
"AntiKtVR60Rmax4Rmin2LCTopoJetsAux"           :   "xAOD::JetAuxContainer"       ,
"AntiKtVR20Rmax4Rmin0TrackJets"               :   "xAOD::JetContainer"          ,
"AntiKtVR20Rmax4Rmin0TrackJetsAux"            :   "xAOD::JetAuxContainer"       ,
"AntiKtVR40Rmax4Rmin0TrackJets"               :   "xAOD::JetContainer"          ,
"AntiKtVR40Rmax4Rmin0TrackJetsAux"            :   "xAOD::JetAuxContainer"       ,
"AntiKtVR60Rmax4Rmin0TrackJets"               :   "xAOD::JetContainer"          ,
"AntiKtVR60Rmax4Rmin0TrackJetsAux"            :   "xAOD::JetAuxContainer"       ,
"AntiKtVR600Rmax10Rmin2TruthJets"             :   "xAOD::JetContainer"          ,
"AntiKtVR600Rmax10Rmin2TruthJetsAux"          :   "xAOD::JetAuxContainer"       ,
"AntiKtVR60Rmax4Rmin2TruthJets"               :   "xAOD::JetContainer"          ,
"AntiKtVR60Rmax4Rmin2TruthJetsAux"            :   "xAOD::JetAuxContainer"       ,
"BTagging_AntiKt4TrackNT"         		:   "xAOD::BTaggingContainer"   ,
"BTagging_AntiKt4TrackNTAux" 			:   "xAOD::BTaggingAuxContainer",
"BTagging_AntiKtVR60Rmax4Rmin2LCTopoNT"         :   "xAOD::BTaggingContainer"   ,
"BTagging_AntiKtVR60Rmax4Rmin2LCTopoNTAux"      :   "xAOD::BTaggingAuxContainer",
"BTagging_AntiKtVR20Rmax4Rmin0TrackNT"          :   "xAOD::BTaggingContainer"   ,
"BTagging_AntiKtVR20Rmax4Rmin0TrackNTAux"       :   "xAOD::BTaggingAuxContainer",
"BTagging_AntiKtVR40Rmax4Rmin0TrackNT"          :   "xAOD::BTaggingContainer"   ,
"BTagging_AntiKtVR40Rmax4Rmin0TrackNTAux"       :   "xAOD::BTaggingAuxContainer",
"BTagging_AntiKtVR60Rmax4Rmin0TrackNT"          :   "xAOD::BTaggingContainer"   ,
"BTagging_AntiKtVR60Rmax4Rmin0TrackNTAux"       :   "xAOD::BTaggingAuxContainer"
}


EXOT16SmartCollections = ["Electrons","Muons", "InDetTrackParticles", "PrimaryVertices"]
EXOT16ExtraVariables = [ "Electrons.author.Medium.Tight.Loose.charge", "Muons.charge"]
EXOT16AllVariables = ["AntiKt10TruthJets", "AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets",
                      "BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo",
                      "TruthParticles", "TruthEvents", "TruthVertices",
                      "CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles","GSFTrackParticles",
                      "CaloCalTopoClusters",
                      "AntiKt2PV0TrackJets", "BTagging_AntiKt2Track", 
                      "AntiKt3PV0TrackJets", "BTagging_AntiKt3Track", 
                      "AntiKt4PV0TrackJets", "BTagging_AntiKt4Track",
                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                      "AntiKtVR600Rmax10Rmin2LCTopoJets", "AntiKtVR600Rmax10Rmin2TruthJets" ,
                      "AntiKtVR60Rmax4Rmin2LCTopoJets"  , "AntiKtVR60Rmax4Rmin2TruthJets" ,
                      "BTagging_AntiKtVR60Rmax4Rmin2LCTopoNT",
                      "BTagging_AntiKt4TrackNT",
                      "AntiKtVR20Rmax4Rmin0TrackJets", "BTagging_AntiKtVR20Rmax4Rmin0TrackNT",
                      "AntiKtVR40Rmax4Rmin0TrackJets", "BTagging_AntiKtVR40Rmax4Rmin0TrackNT",
                      "AntiKtVR60Rmax4Rmin0TrackJets", "BTagging_AntiKtVR60Rmax4Rmin0TrackNT",
                      ]
