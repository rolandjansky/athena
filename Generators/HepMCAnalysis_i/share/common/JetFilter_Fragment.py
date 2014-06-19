## Default truth jet filter setup
## The specified truth jet container must exist

from GeneratorFilters.GeneratorFiltersConf import QCDTruthJetFilter
if "QCDTruthJetFilter" not in topAlg:
    topAlg += QCDTruthJetFilter()
if "QCDTruthJetFilter" not in StreamEVGEN.RequireAlgs:
    StreamEVGEN.RequireAlgs += ["QCDTruthJetFilter"]

topAlg.QCDTruthJetFilter.MinPt = -1.*GeV
topAlg.QCDTruthJetFilter.MaxPt = float(runArgs.ecmEnergy)*GeV
topAlg.QCDTruthJetFilter.MaxEta = 999.
topAlg.QCDTruthJetFilter.TruthJetContainer = "AntiKt6TruthJets"
topAlg.QCDTruthJetFilter.DoShape = False
