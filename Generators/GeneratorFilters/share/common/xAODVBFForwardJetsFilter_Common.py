# common fragment for xAODVBFForwardJets filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

include("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq, 0.4)

from GeneratorFilters.GeneratorFiltersConf import xAODVBFForwardJetsFilter
xAODVBFForwardJetsFilter = xAODVBFForwardJetsFilter("xAODVBFForwardJetsFilter")
filtSeq += xAODVBFForwardJetsFilter

# to modify cuts put into JOs e.g.:
#filtSeq.xAODVBFForwardJetsFilter.JetMinPt=20.*GeV
#filtSeq.xAODVBFForwardJetsFilter.JetMaxEta=5.0
#filtSeq.xAODVBFForwardJetsFilter.NJets=2
#filtSeq.xAODVBFForwardJetsFilter.Jet1MinPt=30.*GeV
#filtSeq.xAODVBFForwardJetsFilter.Jet1MaxEta=5.0
#filtSeq.xAODVBFForwardJetsFilter.Jet2MinPt=20.*GeV
#filtSeq.xAODVBFForwardJetsFilter.Jet2MaxEta=5.0
#filtSeq.xAODVBFForwardJetsFilter.UseOppositeSignEtaJet1Jet2=False
#filtSeq.xAODVBFForwardJetsFilter.TruthJetContainer="AntiKt4TruthJets"
#filtSeq.xAODVBFForwardJetsFilter.LGMinPt=15.*GeV
#filtSeq.xAODVBFForwardJetsFilter.LGMaxEta=2.5
#filtSeq.xAODVBFForwardJetsFilter.DeltaRJLG=0.05
#filtSeq.xAODVBFForwardJetsFilter.RatioPtJLG=0.3
#filtSeq.xAODVBFForwardJetsFilter.UseLeadingJJ=True
#filtSeq.xAODVBFForwardJetsFilter.MassJJ = 300.*GeV
#filtSeq.xAODVBFForwardJetsFilter.DeltaEtaJJ = 3.0



