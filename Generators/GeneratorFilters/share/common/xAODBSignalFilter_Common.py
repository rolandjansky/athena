# common fragment for xAODBSignal filter
# conversion to XAOD, 
# creation of slimmed container containing electrons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODBSignalFilter
xAODBSignalFilter = xAODBSignalFilter("xAODBSignalFilter")  
filtSeq += xAODBSignalFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODBSignalFilter.Cuts_Final_hadrons_switch = True
#filtSeq.xAODBSignalFilter.Cuts_Final_hadrons_pT = 900.0
#filtSeq.xAODBSignalFilter.Cuts_Final_hadrons_eta = 2.6
#filtSeq.xAODBSignalFilter.B_PDGCode = 521
#filtSeq.xAODBSignalFilter.LVL1MuonCutOn = True
#filtSeq.xAODBSignalFilter.LVL2MuonCutOn = True
#filtSeq.xAODBSignalFilter.LVL1MuonCutPT = 3500 
#filtSeq.xAODBSignalFilter.LVL1MuonCutEta = 2.6
#filtSeq.xAODBSignalFilter.LVL2MuonCutPT = 3500
#filtSeq.xAODBSignalFilter.LVL2MuonCutEta = 2.6

