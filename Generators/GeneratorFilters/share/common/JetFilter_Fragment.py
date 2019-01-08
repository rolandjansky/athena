## Default truth jet filter setup
## The specified truth jet container must exist
	
from GeneratorFilters.GeneratorFiltersConf import QCDTruthJetFilter
if "QCDTruthJetFilter" not in filtSeq:
    filtSeq += QCDTruthJetFilter()
	
filtSeq.QCDTruthJetFilter.MaxPt = float(runArgs.ecmEnergy)*GeV


