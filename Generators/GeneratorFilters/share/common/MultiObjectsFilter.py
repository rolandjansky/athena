## Example configuration for MultiObjectsFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "MultiObjectsFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import MultiObjectsFilter
    filtSeq += MultiObjectsFilter()
    pass

"""
MultiObjectsFilter = filtSeq.MultiObjectsFilter
MultiObjectsFilter.PtCut = 10. *Gaudi::Units::GeV
MultiObjectsFilter.EtaCut = 2.7
MultiObjectsFilter.JetPtCut = 10. *Gaudi::Units::GeV
MultiObjectsFilter.JetEtaCut = 5.2
MultiObjectsFilter.UseSumPt = True
MultiObjectsFilter.SumPtCut = 2000. *Gaudi::Units::GeV
MultiObjectsFilter.UseEle = False
MultiObjectsFilter.UseMuo = True
MultiObjectsFilter.UsePho = False
MultiObjectsFilter.UseJet = True
MultiObjectsFilter.TruthJetContainer = "AntiKt4TruthJets"
"""
