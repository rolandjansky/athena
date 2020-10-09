# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from JetRecConfig.StandardJetConstits import jetconstitdic as cst
from .JetDefinition import  JetDefinition
from xAODBase.xAODType import xAODType



# *********************************************************
# Ghost-associated particles for the standard small R jets 
# *********************************************************
standardghosts =  ["Track","MuonSegment","Truth"]


flavourghosts = ["TruthLabel"+ghosttype  for ghosttype in [
    "BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
    "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
    "TausFinal",
    "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
    "Partons",]
]




# *********************************************************
# Modifiers for the standard small R jets 
# *********************************************************
# (use tuples rather than lists to prevent accidental modification)
standardrecomods = ( "Filter:10000","Width","TrackMoments","TrackSumMoments","JVF","JVT","OriginSetPV",
                    "CaloEnergies", )
clustermods      = ("ECPSFrac","ClusterMoments",)# "LArHVCorr" )
truthmods        =  ("PartonTruthLabel","TruthPartonDR","JetDeltaRLabel:5000"  ) # not working well yet ?
pflowmods        = ()



# *********************************************************
# Standard small R jets definitions
# *********************************************************


AntiKt4EMPFlow = JetDefinition("AntiKt",0.4,cst.EMPFlow,
                               ghostdefs = standardghosts , # not working well yet : flavourghosts,
                               modifiers = ("Calib:T0:mc",)+standardrecomods+truthmods, 
                               standardRecoMode = True,                               
                               lock = True
)





AntiKt4LCTopo = JetDefinition("AntiKt",0.4,cst.LCTopoOrigin,
                              ghostdefs = standardghosts, # not working well yet : flavourghosts,,
                              modifiers = standardrecomods+truthmods+clustermods,
                              standardRecoMode = True,
                              lock = True,
)


AntiKt4EMTopo = JetDefinition("AntiKt",0.4,cst.EMTopoOrigin,
                              ghostdefs = standardghosts, # not working well yet : flavourghosts,,
                              modifiers = standardrecomods+truthmods+clustermods,
                              standardRecoMode = True,
                              lock = True,
)

AntiKt4Truth = JetDefinition("AntiKt",0.4, cst.Truth,
                              modifiers = [],
                              standardRecoMode = True,
                              lock = True,
)

AntiKt4TruthWZ = JetDefinition("AntiKt",0.4, cst.TruthWZ,
                               modifiers = [],
                               standardRecoMode = True,
                               lock = True,
)
