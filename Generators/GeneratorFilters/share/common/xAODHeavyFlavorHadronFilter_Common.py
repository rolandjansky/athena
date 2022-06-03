# common fragment for xAODHeavyFlavorHadron filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODHeavyFlavorHadronFilter
xAODHeavyFlavorHadronFilter = xAODHeavyFlavorHadronFilter("xAODHeavyFlavorHadronFilter")  
filtSeq += xAODHeavyFlavorHadronFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODHeavyFlavorHadronFilter.CharmPtMin = 2.0 *GeV
#filtSeq.xAODHeavyFlavorHadronFilter.BottomPtMin = 5.0 * GeV
#filtSeq.xAODHeavyFlavorHadronFilter.CharmEtaMax = 3.0
#filtSeq.xAODHeavyFlavorHadronFilter.BottomEtaMax = 3.0
#filtSeq.xAODHeavyFlavorHadronFilter.cPtMin = 0.0 * GeV
#filtSeq.xAODHeavyFlavorHadronFilter.bPtMin = 0.0 * GeV
#filtSeq.xAODHeavyFlavorHadronFilter.cEtaMax = 5.0
#filtSeq.xAODHeavyFlavorHadronFilter.bEtaMax = 5.0
#filtSeq.xAODHeavyFlavorHadronFilter.JetPtMin = 20.0 * GeV
#filtSeq.xAODHeavyFlavorHadronFilter.JetEtaMax = 2.5
#filtSeq.xAODHeavyFlavorHadronFilter.PDGPtMin = 2.0 * GeV
#filtSeq.xAODHeavyFlavorHadronFilter.PDGEtaMax = 2.5
#filtSeq.xAODHeavyFlavorHadronFilter.PDGID = 0
#filtSeq.xAODHeavyFlavorHadronFilter.PDGAntiParticleToo = True
#filtSeq.xAODHeavyFlavorHadronFilter.RequestCharm = True
#filtSeq.xAODHeavyFlavorHadronFilter.RequestBottom = True
#filtSeq.xAODHeavyFlavorHadronFilter.Request_cQuark = True
#filtSeq.xAODHeavyFlavorHadronFilter.Request_bQuark = True
#filtSeq.xAODHeavyFlavorHadronFilter.RequestSpecificPDGID = False
#filtSeq.xAODHeavyFlavorHadronFilter.RequireTruthJet = False
#filtSeq.xAODHeavyFlavorHadronFilter.DeltaRFromTruth = 0.4
#filtSeq.xAODHeavyFlavorHadronFilter.TruthContainerName = "AntiKt4TruthJets"




