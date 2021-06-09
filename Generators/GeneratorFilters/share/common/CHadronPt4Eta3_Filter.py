##
## Make a HeavyFlavorHadronFilter and steer it to select C Hadrons
## with pT>4 GeV within |eta| < 3
##
if not hasattr(filtSeq, "HeavyFlavorCHadronPt4Eta3_Filter"):
    from GeneratorFilters.GeneratorFiltersConf import HeavyFlavorHadronFilter
    HeavyFlavorCHadronPt4Eta3_Filter = HeavyFlavorHadronFilter(name="HeavyFlavorCHadronPt4Eta3_Filter")

## Default cut params
HeavyFlavorCHadronPt4Eta3_Filter.RequestBottom=False
HeavyFlavorCHadronPt4Eta3_Filter.RequestCharm=True
HeavyFlavorCHadronPt4Eta3_Filter.Request_cQuark=False
HeavyFlavorCHadronPt4Eta3_Filter.Request_bQuark=False
HeavyFlavorCHadronPt4Eta3_Filter.RequestSpecificPDGID=False
HeavyFlavorCHadronPt4Eta3_Filter.RequireTruthJet=False
HeavyFlavorCHadronPt4Eta3_Filter.CharmPtMin=4*GeV
HeavyFlavorCHadronPt4Eta3_Filter.CharmEtaMax=3.0
