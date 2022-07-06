# common fragment for xAODVBFMjjInterval filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

include("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq, 0.4)

from GeneratorFilters.GeneratorFiltersConf import xAODVBFMjjIntervalFilter
xAODVBFMjjIntervalFilter = xAODVBFMjjIntervalFilter("xAODVBFMjjIntervalFilter")
filtSeq += xAODVBFMjjIntervalFilter

# to modify cuts put into JOs e.g.:
filtSeq.xAODVBFMjjIntervalFilter.RapidityAcceptance = 5.0
filtSeq.xAODVBFMjjIntervalFilter.MinSecondJetPT = 15.*GeV
filtSeq.xAODVBFMjjIntervalFilter.MinOverlapPT = 15.*GeV
filtSeq.xAODVBFMjjIntervalFilter.TruthJetContainerName = "AntiKt4TruthJets"
filtSeq.xAODVBFMjjIntervalFilter.NoJetProbability  = 1000.0*0.0000252
filtSeq.xAODVBFMjjIntervalFilter.OneJetProbability = 1000.0*0.0000252
filtSeq.xAODVBFMjjIntervalFilter.LowMjjProbability = 1000.0*0.0000252
filtSeq.xAODVBFMjjIntervalFilter.HighMjjProbability = 1.0
filtSeq.xAODVBFMjjIntervalFilter.LowMjj = 100.*GeV
filtSeq.xAODVBFMjjIntervalFilter.TruncateAtLowMjj = False
filtSeq.xAODVBFMjjIntervalFilter.HighMjj = 2500.*GeV
filtSeq.xAODVBFMjjIntervalFilter.TruncateAtHighMjj = False
filtSeq.xAODVBFMjjIntervalFilter.PhotonJetOverlapRemoval = True
filtSeq.xAODVBFMjjIntervalFilter.ElectronJetOverlapRemoval = True
filtSeq.xAODVBFMjjIntervalFilter.TauJetOverlapRemoval = True




