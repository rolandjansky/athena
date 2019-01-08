## Example configuration for LeadingDiBjetFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "LeadingDiBjetFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import LeadingDiBjetFilter
    filtSeq += LeadingDiBjetFilter()
    pass

"""
LeadingDiBjetFilter = filtSeq.LeadingDiBjetFilter
LeadingDiBjetFilter.LeadJetPtMin = 0 * GeV 
LeadingDiBjetFilter.LeadJetPtMax = 50000 *CLHEP::GeV
LeadingDiBjetFilter.BottomPtMin = 5.0 *CLHEP::GeV
LeadingDiBjetFilter.BottomEtaMax = 3.0
LeadingDiBjetFilter.JetPtMin = 15.0 *CLHEP::GeV
LeadingDiBjetFilter.JetEtaMax = 2.7
LeadingDiBjetFilter.DeltaRFromTruth = 0.3
LeadingDiBjetFilter.TruthContainerName = "AntiKt4TruthJets"
LeadingDiBjetFilter.LightJetSuppressionFactor = 10
LeadingDiBjetFilter.AcceptSomeLightEvents = False
"""
