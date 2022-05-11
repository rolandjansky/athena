# common fragment for xAODVBFMjjInterval filter
# conversion to XAOD, 
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

include("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq, 0.4)

from GeneratorFilters.GeneratorFiltersConf import xAODVBFMjjIntervalFilter
xAODVBFMjjIntervalFilter = xAODVBFMjjIntervalFilter("xAODVBFMjjIntervalFilter")
filtSeq += xAODVBFMjjIntervalFilter

# to modify cuts put into JOs e.g.:
filtSeq.VBFMjjIntervalFilter.RapidityAcceptance = 5.0
filtSeq.VBFMjjIntervalFilter.MinSecondJetPT = 15.*GeV
filtSeq.VBFMjjIntervalFilter.MinOverlapPT = 15.*GeV
filtSeq.VBFMjjIntervalFilter.TruthJetContainerName = "AntiKt4TruthJets"
filtSeq.VBFMjjIntervalFilter.NoJetProbability  = 1000.0*0.0000252
filtSeq.VBFMjjIntervalFilter.OneJetProbability = 1000.0*0.0000252
filtSeq.VBFMjjIntervalFilter.LowMjjProbability = 1000.0*0.0000252
filtSeq.VBFMjjIntervalFilter.HighMjjProbability = 1.0
filtSeq.VBFMjjIntervalFilter.LowMjj = 100.*GeV
filtSeq.VBFMjjIntervalFilter.TruncateAtLowMjj = False
filtSeq.VBFMjjIntervalFilter.HighMjj = 2500.*GeV
filtSeq.VBFMjjIntervalFilter.TruncateAtHighMjj = False
filtSeq.VBFMjjIntervalFilter.PhotonJetOverlapRemoval = True
filtSeq.VBFMjjIntervalFilter.ElectronJetOverlapRemoval = True
filtSeq.VBFMjjIntervalFilter.TauJetOverlapRemoval = True




