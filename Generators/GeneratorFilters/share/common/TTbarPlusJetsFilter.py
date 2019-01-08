## Example configuration for TTbarPlusJetsFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "TTbarPlusJetsFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import TTbarPlusJetsFilter
    filtSeq += TTbarPlusJetsFilter()
    pass
"""
TTbarPlusJetsFilter = filtSeq.TTbarPlusJetsFilter
TTbarPlusJetsFilter.InputJetContainer = "AntiKt4TruthJets"
TTbarPlusJetsFilter.PtMinJet = 14000.0
TTbarPlusJetsFilter.EtaMaxJet = 5.2
TTbarPlusJetsFilter.NbJetMin = 6
TTbarPlusJetsFilter.PtMinJetB = 14000.0
TTbarPlusJetsFilter.EtaMaxJetB = 2.7
TTbarPlusJetsFilter.NbJetBMin = 4
TTbarPlusJetsFilter.PtMinElec = 14000.0
TTbarPlusJetsFilter.EtaMaxElec = 2.7
TTbarPlusJetsFilter.PtMinMuon = 14000.0
TTbarPlusJetsFilter.EtaMaxMuon = 2.7
TTbarPlusJetsFilter.NbLeptonMin = 1
TTbarPlusJetsFilter.SelectLepHadEvents = True # select only lepton(e/mu)+jet W configurations
TTbarPlusJetsFilter.NbEventMax = 0 # 0 means no limit
"""
