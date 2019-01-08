
## Instantiate the lepton filter, including adding it to the stream requirement
## Configuration of the filter cuts is left to the specific JO
if not hasattr(filtSeq, "WZtoLeptonFilter"):
    from GeneratorFilters.GeneratorFiltersConf import WZtoLeptonFilter
    filtSeq += WZtoLeptonFilter("WZtoLeptonFilter")
    
## Add this filter to the algs required to be successful for streaming
#if "WZtoLeptonFilter" not in StreamEVGEN.RequireAlgs:
#    StreamEVGEN.RequireAlgs += ["WZtoLeptonFilter"]

## Cut parameters to keep events with at least one lepton
filtSeq.WZtoLeptonFilter.NeedWZleps = 1
filtSeq.WZtoLeptonFilter.ElectronMuonNumber = 0
filtSeq.WZtoLeptonFilter.BCKGvsSIGNAL = 0
filtSeq.WZtoLeptonFilter.IdealReconstructionEfficiency = 0
filtSeq.WZtoLeptonFilter.Etacut_electron = 2.6
filtSeq.WZtoLeptonFilter.Etacut_muon = 2.6
