##
## Make a HeavyFlavorHadronFilter and steer it to select at least 
## one AntiKt jet with pT>15 GeV, |eta|<3 containing stable 
## C Hadrons with pT>4 GeV matched within DR<0.5
##
if not hasattr(filtSeq, "HeavyFlavorCHadronPt4Eta4_JetFilter"):
    from GeneratorFilters.GeneratorFiltersConf import HeavyFlavorHadronFilter
    HeavyFlavorCHadronPt4Eta4_JetFilter = HeavyFlavorHadronFilter(name="HeavyFlavorCHadronPt4Eta4_JetFilter")

# Anti-kt truth jets with pt>7 GeV and R=0.4
include("GeneratorFilters/AntiKt4TruthJets.py")
#
HeavyFlavorCHadronPt4Eta4_JetFilter.RequestBottom=False
HeavyFlavorCHadronPt4Eta4_JetFilter.RequestCharm=True
HeavyFlavorCHadronPt4Eta4_JetFilter.Request_cQuark=False
HeavyFlavorCHadronPt4Eta4_JetFilter.Request_bQuark=False
HeavyFlavorCHadronPt4Eta4_JetFilter.RequestSpecificPDGID=False
HeavyFlavorCHadronPt4Eta4_JetFilter.RequireTruthJet=True
HeavyFlavorCHadronPt4Eta4_JetFilter.CharmPtMin=4*GeV
HeavyFlavorCHadronPt4Eta4_JetFilter.CharmEtaMax=4.0
#
HeavyFlavorCHadronPt4Eta4_JetFilter.DeltaRFromTruth=0.5
HeavyFlavorCHadronPt4Eta4_JetFilter.JetPtMin=15.0*GeV
HeavyFlavorCHadronPt4Eta4_JetFilter.JetEtaMax=3.0
HeavyFlavorCHadronPt4Eta4_JetFilter.TruthContainerName="AntiKt4TruthJets"

