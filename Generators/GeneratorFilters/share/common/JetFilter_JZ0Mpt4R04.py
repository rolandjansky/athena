## Truth jet filter config for JZ0 with R=0.4 and MinPt=4GeV
include("GeneratorFilters/JetFilterAkt4.py")
filtSeq.QCDTruthJetFilter.MinPt = 4.*GeV
filtSeq.QCDTruthJetFilter.MaxPt = 20.*GeV
