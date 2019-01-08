##
## Make a HeavyFlavorHadronFilter and steer it to select B Hadrons
## with any pT within |eta| < 4
##
if not hasattr(filtSeq, "HeavyFlavorBHadronFilter"):
    from GeneratorFilters.GeneratorFiltersConf import HeavyFlavorHadronFilter
    HeavyFlavorBHadronFilter = HeavyFlavorHadronFilter(name="HeavyFlavorBHadronFilter")

## Default cut params
HeavyFlavorBHadronFilter.RequestBottom=True
HeavyFlavorBHadronFilter.RequestCharm=False
HeavyFlavorBHadronFilter.Request_cQuark=False
HeavyFlavorBHadronFilter.Request_bQuark=False
HeavyFlavorBHadronFilter.RequestSpecificPDGID=False
HeavyFlavorBHadronFilter.RequireTruthJet=False
HeavyFlavorBHadronFilter.BottomPtMin=0*GeV
HeavyFlavorBHadronFilter.BottomEtaMax=4.0

