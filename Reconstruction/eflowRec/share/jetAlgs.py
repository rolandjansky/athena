#Sechdule origin corrections of the rebuilt topoclusters
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm
topSequence += JetAlgorithm("lctoporigin", Tools= [jtm.JetConstitSeq_LCOrigin] )
topSequence += JetAlgorithm("emtoporigin", Tools= [jtm.JetConstitSeq_EMOrigin] )

#Run small R jet finding
jtm.addJetFinder("AntiKt4EMPFlowJets",  "AntiKt", 0.4,  "empflow_reduced", "pflow_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter= 10000, calibOpt="arj:pflow")
jtm.addJetFinder("AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo_reduced", "emtopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter=15000, calibOpt="arj")
jtm.addJetFinder("AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo_reduced", "lctopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter=15000, calibOpt="arj")

from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(eventShapeTools=["empflow","emtopo","lctopo"])

if not hasattr( topSequence, "LumiBlockMuWriter" ):
    include ("LumiBlockComps/LumiBlockMuWriter_jobOptions.py")
