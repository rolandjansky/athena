# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from JetRecConfig.StandardJetConstits import stdConstitDic as cst
from .JetDefinition import  JetDefinition
from .JetGrooming import  JetTrimming, JetSoftDrop

# needed to ensure the smallR VR jets are defined
import JetRecConfig.StandardSmallRJets # noqa: F401

# *********************************************************
# Ghost-associated particles for the standard large R jets 
# *********************************************************
standardghosts =  ["Track","MuonSegment","Truth"]


flavourghosts = [ "BHadronsFinal",
                  "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
                  "Partons",]





# *********************************************************
# Modifiers for the standard large R jets 
# *********************************************************
# (use tuples rather than lists to prevent accidental modification)
standardrecomods = (
    "Sort",
    "Width",
    "ConstitFourMom"
)
clustermods      = ("ECPSFrac","ClusterMoments",) 
truthmods        = ("PartonTruthLabel","TruthPartonDR",)
pflowmods        = ()

truthlabels = ("JetTaggingTruthLabel:R10TruthLabel_R21Consolidated","JetTaggingTruthLabel:R10TruthLabel_R21Precision",)

substrmods = ("nsubjettiness", "nsubjettinessR", "ktsplitter",
              "ecorr", "ecorrR", "qw",
              # ... others ?
)

#Variables used for trimmed large-R jets
lctopo_trimmed_mods = ("planarflow","angularity","comshapes","ktdr","TrackSumMoments")


# *********************************************************
# Standard large R jets definitions
# *********************************************************


AntiKt10LCTopo = JetDefinition("AntiKt",1.0,cst.LCTopoOrigin,
                               ghostdefs = standardghosts+flavourghosts+["AntiKtVR30Rmax4Rmin02PV0TrackJets"] ,
                               modifiers = ("Sort", "Filter:50000","TrackMoments"),
                               standardRecoMode = True,                               
                               lock = True
)

#Remove VR track jets from ghosts for core reco
AntiKt10LCTopo_noVR = AntiKt10LCTopo.clone(
    ghostdefs = standardghosts+flavourghosts
)

AntiKt10LCTopo_withmoms = AntiKt10LCTopo.clone(
    modifiers = ("Sort", "Filter:50000", "Width", "TrackMoments", "TrackSumMoments","JetDeltaRLabel:5000")+clustermods+truthmods,
    # NOT all moments from old AntiKt10LCTopo config here yet. 
)

AntiKt10LCTopoTrimmed = JetTrimming(AntiKt10LCTopo,
                                    modifiers = ("Calib:CombinedMass:mc","Filter:100000")+standardrecomods+substrmods+lctopo_trimmed_mods+truthlabels,
                                    PtFrac = 0.05, RClus = 0.2,                                    
                                    )

AntiKt10LCTopoSoftDrop = JetSoftDrop(AntiKt10LCTopo,
                                     modifiers = standardrecomods+substrmods,
                                     Beta = 1., ZCut= 0.1,
                                     )


AntiKt10Truth = JetDefinition("AntiKt",1.0,cst.Truth,
                               ghostdefs = flavourghosts , 
                               modifiers = ("Sort", "Filter:50000","ktsplitter"),
                               standardRecoMode = True,                               
                               lock = True
)


AntiKt10TruthTrimmed = JetTrimming(AntiKt10Truth,
                                   modifiers = ("Sort",)+substrmods+truthmods,
                                   PtFrac = 0.05, RClus = 0.2,                                    
                                   )

AntiKt10TruthSoftDrop = JetSoftDrop(AntiKt10Truth,
                                    modifiers = ("Sort",)+substrmods+truthmods,
                                    Beta = 1., ZCut= 0.1,
                                    )






        
