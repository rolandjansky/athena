# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from JetRecConfig.StandardJetConstits import stdConstitDic as cst
from .JetDefinition import  JetDefinition
from .JetGrooming import  JetTrimming, JetSoftDrop



# *********************************************************
# Ghost-associated particles for the standard large R jets 
# *********************************************************
standardghosts =  ["Track","MuonSegment","Truth"]


flavourghosts = [
                  "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
                  "Partons",]





# *********************************************************
# Modifiers for the standard large R jets 
# *********************************************************
# (use tuples rather than lists to prevent accidental modification)
standardrecomods = (
    "Sort",
    "Width",
    "TrackMoments",
)
clustermods      = ("ECPSFrac","ClusterMoments",) 
truthmods        =  ("PartonTruthLabel","TruthPartonDR", ) 
pflowmods        = ()

substrmods = ("nsubjettiness", "nsubjettinessR", "ktsplitter",
              "ecorr", "ecorrR",  "qw",
              # ... others ?
)


# *********************************************************
# Standard large R jets definitions
# *********************************************************


AntiKt10LCTopo = JetDefinition("AntiKt",1.0,cst.LCTopoOrigin,
                               ghostdefs = standardghosts+flavourghosts , 
                               modifiers = ("Sort", "Filter:50000"), 
                               standardRecoMode = True,                               
                               lock = True
)


AntiKt10LCTopoTrimmed = JetTrimming(AntiKt10LCTopo,
                                    modifiers = standardrecomods+substrmods,
                                    PtFrac = 0.05, RClus = 0.2,                                    
                                    )

AntiKt10LCTopoSoftDrop = JetSoftDrop(AntiKt10LCTopo,
                                     modifiers = standardrecomods+substrmods,
                                     Beta = 1., ZCut= 0.1,
                                     )




AntiKt10Truth = JetDefinition("AntiKt",1.0,cst.Truth,
                               ghostdefs = flavourghosts , 
                               modifiers = ("Sort", "Filter:50000"), 
                               standardRecoMode = True,                               
                               lock = True
)


AntiKt10TruthTrimmed = JetTrimming(AntiKt10Truth,
                                   modifiers = standardrecomods+substrmods+truthmods,
                                   PtFrac = 0.05, RClus = 0.2,                                    
                                   )

AntiKt10TruthSoftDrop = JetSoftDrop(AntiKt10Truth,
                                    modifiers = standardrecomods+substrmods+truthmods,
                                    Beta = 1., ZCut= 0.1,
                                    )






        
