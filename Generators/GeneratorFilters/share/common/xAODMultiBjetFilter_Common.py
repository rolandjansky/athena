# common fragment for xAODMultiBjet filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# creating truh jets container
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

include("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq, 0.4)

from GeneratorFilters.GeneratorFiltersConf import xAODMultiBjetFilter
xAODMultiBjetFilter = xAODMultiBjetFilter("xAODMultiBjetFilter")  
filtSeq += xAODMultiBjetFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODMultiBjetFilter.LeadJetPtMin = 0
#filtSeq.xAODMultiBjetFilter.LeadJetPtMax = 50000
#filtSeq.xAODMultiBjetFilter.BottomPtMin = 5.0
#filtSeq.xAODMultiBjetFilter.BottomEtaMax = 3.0
#filtSeq.xAODMultiBjetFilter.JetPtMin = 13000
#filtSeq.xAODMultiBjetFilter.JetEtaMax = 2.7
#filtSeq.xAODMultiBjetFilter.DeltaRFromTruth = 0.3
#filtSeq.xAODMultiBjetFilter.TruthContainerName = "AntiKt4TruthJets"
#filtSeq.xAODMultiBjetFilter.NJetsMin = 4
#filtSeq.xAODMultiBjetFilter.NBJetsMin = 2


