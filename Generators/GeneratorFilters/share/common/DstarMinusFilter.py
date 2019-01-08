## Filter on presence of Dstar hadrond

if not hasattr( filtSeq, "HeavyFlavorHadronFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import HeavyFlavorHadronFilter
    filtSeq += HeavyFlavorHadronFilter()
    pass
HeavyFlavorHadronFilter = filtSeq.HeavyFlavorHadronFilter
HeavyFlavorHadronFilter.RequestCharm=FALSE
HeavyFlavorHadronFilter.RequestBottom=FALSE
HeavyFlavorHadronFilter.Request_cQuark=FALSE
HeavyFlavorHadronFilter.Request_bQuark=FALSE
HeavyFlavorHadronFilter.RequestSpecificPDGID=TRUE
HeavyFlavorHadronFilter.PDGID=-413
HeavyFlavorHadronFilter.PDGAntiParticleToo=FALSE
HeavyFlavorHadronFilter.RequireTruthJet=FALSE
HeavyFlavorHadronFilter.PDGEtaMax=3.0
HeavyFlavorHadronFilter.PDGPtMin=4.0*GeV
HeavyFlavorHadronFilter.RequireTruthJet = False
