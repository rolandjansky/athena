## Example configuration for HeavyFlavorHadronFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "HeavyFlavorHadronFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import HeavyFlavorHadronFilter
    filtSeq += HeavyFlavorHadronFilter()
    pass

# Setup defaults as example
"""
HeavyFlavorHadronFilter = filtSeq.HeavyFlavorHadronFilter
HeavyFlavorHadronFilter.CharmPtMin = 2.0 * Gaudi::Units::GeV
HeavyFlavorHadronFilter.BottomPtMin = 5.0 * Gaudi::Units::GeV
HeavyFlavorHadronFilter.CharmEtaMax = 3.0
HeavyFlavorHadronFilter.BottomEtaMax = 3.0
HeavyFlavorHadronFilter.cPtMin = 0.0 * Gaudi::Units::GeV
HeavyFlavorHadronFilter.bPtMin = 0.0 * Gaudi::Units::GeV
HeavyFlavorHadronFilter.cEtaMax = 5.0
HeavyFlavorHadronFilter.bEtaMax = 5.0
HeavyFlavorHadronFilter.JetPtMin = 20.0 * Gaudi::Units::GeV
HeavyFlavorHadronFilter.JetEtaMax = 2.5
HeavyFlavorHadronFilter.PDGPtMin = 2.0 * Gaudi::Units::GeV
HeavyFlavorHadronFilter.PDGEtaMax = 2.5
HeavyFlavorHadronFilter.PDGID = 0
HeavyFlavorHadronFilter.PDGAntiParticleToo = True
HeavyFlavorHadronFilter.RequestCharm = True
HeavyFlavorHadronFilter.RequestBottom = True
HeavyFlavorHadronFilter.Request_cQuark = True
HeavyFlavorHadronFilter.Request_bQuark = True
HeavyFlavorHadronFilter.RequestSpecificPDGID = False
HeavyFlavorHadronFilter.RequireTruthJet = False
HeavyFlavorHadronFilter.DeltaRFromTruth = 0.4
HeavyFlavorHadronFilter.TruthContainerName = "AntiKt4TruthJets"
"""
