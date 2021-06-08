##
## Make a HeavyFlavorHadronFilter and steer it to select C Hadrons
## with any pT within |eta| < 4
##
if not hasattr(filtSeq, "HeavyFlavorCHadronFilter"):
    from GeneratorFilters.GeneratorFiltersConf import HeavyFlavorHadronFilter
    HeavyFlavorCHadronFilter = HeavyFlavorHadronFilter(name="HeavyFlavorCHadronFilter")

## Default cut params
HeavyFlavorCHadronFilter.RequestBottom=False
HeavyFlavorCHadronFilter.RequestCharm=True
HeavyFlavorCHadronFilter.Request_cQuark=False
HeavyFlavorCHadronFilter.Request_bQuark=False
HeavyFlavorCHadronFilter.RequestSpecificPDGID=False
HeavyFlavorCHadronFilter.RequireTruthJet=False
HeavyFlavorCHadronFilter.CharmPtMin=0*GeV
HeavyFlavorCHadronFilter.CharmEtaMax=4.0
