## Example configuration for DiBJet filter setting up defaults                                                                                                                                   

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "DiBjetFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import DiBjetFilter
    filtSeq += DiBjetFilter()
    pass

"""
DiBjetFilter = filtSeq.DiBjetFilter
DiBjetFilter.LeadJetPtMin = 0
DiBjetFilter.LeadJetPtMax = 50000
DiBjetFilter.BottomPtMin = 5.0
DiBjetFilter.BottomEtaMax = 3.0
DiBjetFilter.JetPtMin = 15.0
DiBjetFilter.JetEtaMax = 2.7
DiBjetFilter.DeltaRFromTruth = 0.3
DiBjetFilter.TruthContainerName = "AntiKt4TruthJets" 
DiBjetFilter.LightJetSuppressionFactor = 10
DiBjetFilter.AcceptSomeLightEvents = False
"""
