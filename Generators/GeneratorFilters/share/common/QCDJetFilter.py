## Default truth jet filter setup
## The specified truth jet container must exist

def AddJetFilter(filtSeq,ecmEnergy):	
    from GeneratorFilters.GeneratorFiltersConf import QCDTruthJetFilter
    if "QCDTruthJetFilter" not in filtSeq:
        from AthenaCommon.SystemOfUnits import GeV
        filtSeq += QCDTruthJetFilter()
        filtSeq.QCDTruthJetFilter.MaxPt = float(ecmEnergy)*GeV


