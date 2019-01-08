## Example configuration for VBFMjjIntervalFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "VBFMjjIntervalFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import VBFMjjIntervalFilter
    filtSeq += VBFMjjIntervalFilter()
    pass

"""
VBFMjjIntervalFilter = filtSeq.VBFMjjIntervalFilter
VBFMjjIntervalFilter.RapidityAcceptance = 5.0
VBFMjjIntervalFilter.MinSecondJetPT = 15.0 *CLHEP::GeV
VBFMjjIntervalFilter.MinOverlapPT = 15.0 *CLHEP::GeV
VBFMjjIntervalFilter.TruthJetContainerName = "AntiKt4TruthJets"
VBFMjjIntervalFilter.NoJetProbability = 0.0002
VBFMjjIntervalFilter.OneJetProbability = 0.001
VBFMjjIntervalFilter.LowMjjProbability = 0.005
VBFMjjIntervalFilter.HighMjjProbability = 1.0
VBFMjjIntervalFilter.LowMjj = 100.0 *CLHEP::GeV
VBFMjjIntervalFilter.HighMjj = 800.0 *CLHEP::GeV
"""
