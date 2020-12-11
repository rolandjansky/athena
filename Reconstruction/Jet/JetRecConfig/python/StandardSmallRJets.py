# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from JetRecConfig.StandardJetConstits import jetconstitdic as cst
from .JetDefinition import  JetDefinition



# *********************************************************
# Ghost-associated particles for the standard small R jets 
# *********************************************************
standardghosts =  ["Track","MuonSegment","Truth"]


flavourghosts = [ "BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                  "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                  "TausFinal",
                  "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
                  "Partons",]





# *********************************************************
# Modifiers for the standard small R jets 
# *********************************************************
# (use tuples rather than lists to prevent accidental modification)
standardrecomods = (
    "ConstitFourMom",   "CaloEnergies",
    "Calib:T0:mc",
    "Sort","Filter:10000",
    "LArHVCorr", "Width",
     "CaloQuality", "TrackMoments","TrackSumMoments",
    "JVF","JVT","OriginSetPV", "Charge",
)
clustermods      = ("ECPSFrac","ClusterMoments",) 
truthmods        =  ("PartonTruthLabel","TruthPartonDR","JetDeltaRLabel:5000"  ) 
pflowmods        = ()



# *********************************************************
# Standard small R jets definitions
# *********************************************************


AntiKt4EMPFlow = JetDefinition("AntiKt",0.4,cst.EMPFlow,
                               ghostdefs = standardghosts+flavourghosts , 
                               modifiers = standardrecomods+truthmods, 
                               standardRecoMode = True,                               
                               lock = True
)




AntiKt4LCTopo = JetDefinition("AntiKt",0.4,cst.LCTopoOrigin,
                              ghostdefs = standardghosts+flavourghosts, 
                              modifiers = standardrecomods+truthmods+clustermods,
                              standardRecoMode = True,
                              lock = True,
)


AntiKt4EMTopo = JetDefinition("AntiKt",0.4,cst.EMTopoOrigin,
                              ghostdefs = standardghosts+flavourghosts, 
                              modifiers = standardrecomods+truthmods+clustermods,
                              standardRecoMode = True,
                              lock = True,
)

AntiKt4Truth = JetDefinition("AntiKt",0.4, cst.Truth,
                             ghostdefs = flavourghosts,
                             modifiers = ("Sort","Filter:10000", "Width")+truthmods,
                             standardRecoMode = True,
                             lock = True,
)

AntiKt4TruthWZ = JetDefinition("AntiKt",0.4, cst.TruthWZ,
                               ghostdefs = flavourghosts,
                               modifiers = ("Sort","Filter:10000", "Width")+truthmods,
                               standardRecoMode = True,
                               lock = True,
)





def StandardSmallRJetCfg(configFlags):
    """Top-level function to schedule the smallR jets in standard reconstruction """
    from JetRecConfig.JetRecConfig import JetRecCfg

    standarSmallRList = [
        AntiKt4EMPFlow,
        AntiKt4LCTopo,
        AntiKt4Truth,
        ]

    compacc = JetRecCfg( standarSmallRList[0], configFlags)
    for jetdef in standarSmallRList[1:]:
        compacc.merge( JetRecCfg(jetdef, configFlags) )

    return compacc
        
