## Example configuration for JetIntervalFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "JetIntervalFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import JetIntervalFilter
    filtSeq += JetIntervalFilter()
    pass
"""
JetIntervalFilter = filtSeq.JetIntervalFilter
# General Jets
JetIntervalFilter.JetContainer = "AntiKt4TruthJets"
JetIntervalFilter.JetNumber = 2
JetIntervalFilter.OppositeSideJets = False

#  Jet Energy Cuts (give choice of et or pt cutting)
JetIntervalFilter.JetCutByPt = True
JetIntervalFilter.Jet1MinEt = 10.0 * CLHEP::GeV
JetIntervalFilter.Jet1MaxEt = 1000.0 * CLHEP::GeV
JetIntervalFilter.Jet1MinPt = 10.0 * CLHEP::GeV
JetIntervalFilter.Jet1MaxPt = 1000.0 * CLHEP::GeV
JetIntervalFilter.Jet2MinEt = 10.0 * CLHEP::GeV
JetIntervalFilter.Jet2MaxEt = 1000.0 * CLHEP::GeV
JetIntervalFilter.Jet2MinPt = 10.0 * CLHEP::GeV
JetIntervalFilter.Jet2MaxPt = 1000.0 * CLHEP::GeV

#  Jet Position Cuts (absolute)
JetIntervalFilter.Jet1MaxEta = 100.0
JetIntervalFilter.Jet1MinEta = 0.0
JetIntervalFilter.Jet2MaxEta = 100.0
JetIntervalFilter.Jet2MinEta = 0.0

#  Jet delta eta cuts
JetIntervalFilter.MinDeltaEta = 0.0
JetIntervalFilter.MaxDeltaEta = 100.0

#  Allow filtering with weight storage
#  Default only good for J2 sample
JetIntervalFilter.WeightEvents = False 
JetIntervalFilter.UniformMaxWeightBelowGausMean = False 
JetIntervalFilter.GausMean = 0.0611
JetIntervalFilter.GausSigma = 2.33 
JetIntervalFilter.AbsDEtaCutOff = 8.5 # Above which keep all which are generated
"""
