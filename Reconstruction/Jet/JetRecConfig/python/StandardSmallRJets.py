# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from JetRecConfig.StandardJetConstits import stdConstitDic as cst
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
calibmods = (
    "ConstitFourMom",   "CaloEnergies",
    "Calib:T0:mc",
    "Sort",
    )
standardmods = (
    "LArHVCorr", "Width",
     "CaloQuality", "TrackMoments","TrackSumMoments",
    "JVF","JVT","OriginSetPV", "Charge",
)
clustermods      = ("ECPSFrac","ClusterMoments",) 
truthmods        =  ("PartonTruthLabel","TruthPartonDR","JetDeltaRLabel:5000"  ) 
pflowmods        = ()


# ********************************************************
# Standard VR track jet definition
# ********************************************************
AntiKtVR30Rmax4Rmin02PV0TrackJets = JetDefinition("AntiKt", 0.4, cst.PV0Track,
                               ptmin=4000,
                               standardRecoMode = True,
                               VRMinR = 0.02,
                               VRMassSc = 30000,
                               lock = True)


# *********************************************************
# Standard small R jets definitions
# *********************************************************


AntiKt4EMPFlow = JetDefinition("AntiKt",0.4,cst.EMPFlow,
                               ghostdefs = standardghosts+flavourghosts , 
                               modifiers = calibmods+("Filter:10000",)+standardmods, 
                               standardRecoMode = True,                               
                               lock = True
)




AntiKt4LCTopo = JetDefinition("AntiKt",0.4,cst.LCTopoOrigin,
                              ghostdefs = standardghosts+flavourghosts, 
                              modifiers = calibmods+("Filter:15000",)+standardmods+clustermods,
                              standardRecoMode = True,
                              lock = True,
)



AntiKt4EMTopo = JetDefinition("AntiKt",0.4,cst.EMTopoOrigin,
                              ghostdefs = standardghosts+flavourghosts, 
                              modifiers = calibmods+("Filter:15000",)+standardmods+clustermods,
                              standardRecoMode = True,
                              lock = True,
)

AntiKt4Truth = JetDefinition("AntiKt",0.4, cst.Truth,
                             ghostdefs = flavourghosts,
                             modifiers = ("Sort", "Width")+truthmods,
                             standardRecoMode = True,
                             lock = True,
)

AntiKt4TruthWZ = JetDefinition("AntiKt",0.4, cst.TruthWZ,
                               ghostdefs = flavourghosts,
                               modifiers = ("Sort", "Width")+truthmods,
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

    compacc = JetRecCfg( configFlags, standarSmallRList[0], )
    for jetdef in standarSmallRList[1:]:
        compacc.merge( JetRecCfg( configFlags, jetdef) )

    return compacc
        
