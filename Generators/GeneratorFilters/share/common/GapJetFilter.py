## Example configuration for GapJetFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "GapJetFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import GapJetFilter
    filtSeq += GapJetFilter()
    pass

"""
GapJetFilter = filtSeq.GapJetFilter
GapJetFilter.JetContainer = "AntiKt4TruthJets"
  
#Allow a hard cut on the jet system
GapJetFilter.MinPt1 = 12.0    # In GeV
GapJetFilter.MaxPt1 = 70000.0 # In GeV
GapJetFilter.MinPt2 = 12.0    # In GeV
GapJetFilter.MaxPt2 = 70000.0 # In GeV

GapJetFilter.MinPtparticle = 0. # In MeV
GapJetFilter.MaxEtaparticle = 0.
 
GapJetFilter.c0 = 0.
GapJetFilter.c1 = 0.
GapJetFilter.c2 = 0.
GapJetFilter.c3 = 0.
GapJetFilter.c4 = 0.
GapJetFilter.c5 = 0. 
GapJetFilter.c6 = 0.
GapJetFilter.c7 = 0.
GapJetFilter.gapf = 0.  
"""
