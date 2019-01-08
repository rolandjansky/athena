## Example configuration for BSubtract filter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "BSubstrcut" ):
    from GeneratorFilters.GeneratorFiltersConf import BSubstruct
    filtSeq += BSubstruct()
    pass

"""
BSubstructFilter = filtSeq.BSubstruct
BSubstructFilter.JetKey = "AntiKt4TruthJets"
BSubstructFilter.pTMin = 180
BSubstructFilter.pTMax = -1
BSubstructFilter.filterB = True
BSubstructFilter.filterC = True
BSubstructFilter.dRMatch = 1.2
BSubstructFilter.doHistos = False
"""
