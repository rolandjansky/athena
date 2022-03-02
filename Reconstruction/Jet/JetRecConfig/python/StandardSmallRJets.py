# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from JetRecConfig.StandardJetConstits import stdConstitDic as cst
from JetRecConfig.JetRecConfig import registerAsInputConstit
from .JetDefinition import  JetDefinition


# *********************************************************
# Ghost-associated particles for the standard small R jets 
# *********************************************************
standardghosts =  ["Track","MuonSegment","Truth","Tower"]


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

calibmods_lowCut = (
    "ConstitFourMom", "CaloEnergies",
    "Calib:T0:mc:JetArea_Residual",
    "Sort",
)

standardmods = (
    "LArHVCorr", "Width",
     "CaloQuality", "TrackMoments","TrackSumMoments",
    "JVF", "JVT", "Charge",
)

clustermods      = ("ECPSFrac","ClusterMoments",) 
truthmods        = ("PartonTruthLabel","TruthPartonDR","JetDeltaRLabel:5000")
pflowmods        = ()


# ********************************************************
# Standard track jet definition
# ********************************************************
AntiKtVR30Rmax4Rmin02PV0Track = JetDefinition("AntiKt", 0.4, cst.PV0Track,
                                              modifiers = ("Sort","JetDeltaRLabel:4500","JetGhostLabel"),
                                              ptmin=4000,
                                              standardRecoMode = True,
                                              VRMinR = 0.02,
                                              VRMassSc = 30000,
                                              lock = True)

# These jets are used as ghost, so they also need to be defined as constituents : 
registerAsInputConstit(AntiKtVR30Rmax4Rmin02PV0Track)


AntiKt4PV0Track = JetDefinition("AntiKt", 0.4, cst.PV0Track,
                                modifiers = ("Sort",)+truthmods,
                                ptmin=2000,
                                standardRecoMode = True,
                                lock = True)


# *********************************************************
# Standard small R reco jet definitions
# *********************************************************


AntiKt4EMPFlow = JetDefinition("AntiKt",0.4,cst.EMPFlow,
                               ghostdefs = standardghosts+flavourghosts,
                               modifiers = calibmods+("Filter:10000",)+truthmods+standardmods,
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
                              ghostdefs = standardghosts+["TrackLRT"]+flavourghosts,
                              modifiers = calibmods+("Filter:15000",)+truthmods+standardmods+clustermods,
                              standardRecoMode = True,
                              lock = True,
)

# *********************************************************
# EMPFlow CSSK jets
# *********************************************************
AntiKt4EMPFlowCSSK = JetDefinition("AntiKt",0.4,cst.EMPFlowCSSK,
                                   ghostdefs = standardghosts+flavourghosts,
                                   modifiers = ("ConstitFourMom","CaloEnergies","Sort","Filter:1",)+truthmods+standardmods,
                                   ptmin = 2000,
                                   standardRecoMode = True,
                                   lock = True
)

# *********************************************************
# Low and no pT cut containers used in JETMX derivations
# *********************************************************
AntiKt4EMPFlowNoPtCut = JetDefinition("AntiKt",0.4,cst.EMPFlow,
                                      infix = "NoPtCut",
                                      ghostdefs = standardghosts+flavourghosts,
                                      modifiers = calibmods_lowCut+("Filter:2000",)+truthmods+standardmods,
                                      ptmin = 1,
                                      standardRecoMode = True,
                                      lock = True
)


AntiKt4EMTopoNoPtCut = JetDefinition("AntiKt",0.4,cst.EMTopoOrigin,
                                     infix = "NoPtCut",
                                     ghostdefs = standardghosts+flavourghosts,
                                     modifiers = calibmods_lowCut+("Filter:1",)+truthmods+standardmods+clustermods,
                                     ptmin = 1,
                                     standardRecoMode = True,
                                     lock = True
)

AntiKt4EMPFlowLowPt = JetDefinition("AntiKt",0.4,cst.EMPFlow,
                                    infix = "LowPt",
                                    ghostdefs = standardghosts+flavourghosts,
                                    modifiers = calibmods_lowCut+("Filter:2000",)+truthmods+standardmods,
                                    ptmin = 2000,
                                    standardRecoMode = True,
                                    lock = True
)

AntiKt4EMTopoLowPt = JetDefinition("AntiKt",0.4,cst.EMTopoOrigin,
                                   infix = "LowPt",
                                   ghostdefs = standardghosts+flavourghosts,
                                   modifiers = calibmods_lowCut+("Filter:2000",)+truthmods+standardmods+clustermods,
                                   ptmin = 2000,
                                   standardRecoMode = True,
                                   lock = True
)

# *********************************************************
# Standard small R truth jet definitions
# *********************************************************

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

AntiKt4TruthDressedWZ = JetDefinition("AntiKt",0.4, cst.TruthDressedWZ,
                                      ghostdefs = flavourghosts,
                                      modifiers = ("Sort", "Width")+truthmods,
                                      standardRecoMode = True,
                                      lock = True,
)

AntiKtVRTruthCharged = JetDefinition("AntiKt",0.4, cst.TruthCharged,
                                     ghostdefs = flavourghosts,
                                     modifiers = ("Sort",)+truthmods,
                                     VRMinR = 0.02,
                                     VRMassSc = 30000,
                                     standardRecoMode = True,
                                     lock = True
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
        
