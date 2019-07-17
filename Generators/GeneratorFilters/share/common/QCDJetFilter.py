## Default truth jet filter setup
## The specified truth jet container must exist

def AddJetFilter(filtSeq,ecmEnergy):	
    from GeneratorFilters.GeneratorFiltersConf import QCDTruthJetFilter
    if "QCDTruthJetFilter" not in filtSeq:
        print "nie ma jeszcze filtra wiec dodamy !"
        from AthenaCommon.SystemOfUnits import GeV
        filtSeq += QCDTruthJetFilter()
        filtSeq.QCDTruthJetFilter.MaxPt = float(ecmEnergy)*GeV


