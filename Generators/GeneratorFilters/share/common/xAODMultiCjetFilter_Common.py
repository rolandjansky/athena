# common fragment for xAODMultiCjet filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# creating truh jets container
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

include("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq, 0.4)

from GeneratorFilters.GeneratorFiltersConf import xAODMultiCjetFilter
xAODMultiCjetFilter = xAODMultiCjetFilter("xAODMultiCjetFilter")  
filtSeq += xAODMultiCjetFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODMultiCjetFilter.LeadJetPtMin = 0
#filtSeq.xAODMultiCjetFilter.LeadJetPtMax = 50000
#filtSeq.xAODMultiCjetFilter.BottomPtMin = 5.0
#filtSeq.xAODMultiCjetFilter.BottomEtaMax = 3.0
#filtSeq.xAODMultiCjetFilter.JetPtMin = 13000
#filtSeq.xAODMultiCjetFilter.JetEtaMax = 2.7
#filtSeq.xAODMultiCjetFilter.DeltaRFromTruth = 0.3
#filtSeq.xAODMultiCjetFilter.TruthContainerName = "AntiKt4TruthJets"
#filtSeq.xAODMultiCjetFilter.NJetsMin = 2
#filtSeq.xAODMultiCjetFilter.NJetsMax = 4
#filtSeq.xAODMultiCjetFilter.NCJetsMin = 1
#filtSeq.xAODMultiCjetFilter.NCJetsMax = 2


