## HT filter setup for anti-kT R=0.4 truth jets
#include("GeneratorFilters/AntiKt4TruthWZJets.py")
include ("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq, 0.4,"WZ")

from GeneratorFilters.GeneratorFiltersConf import HTFilter
if "HTFilter" not in filtSeq:
    filtSeq += HTFilter()

filtSeq.HTFilter.MinJetPt = 20.*GeV # Min pT to consider jet in HT
filtSeq.HTFilter.MaxJetEta = 999. # Max eta to consider jet in HT
filtSeq.HTFilter.MinHT = 200.*GeV # Min HT to keep event
filtSeq.HTFilter.MaxHT = 1000.*GeV # Max HT to keep event
filtSeq.HTFilter.TruthJetContainer = "AntiKt4WZTruthJets" # Which jets to use for HT
filtSeq.HTFilter.UseNeutrinosFromWZTau = False # Include neutrinos from W/Z/tau in the HT
filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from W/Z/tau in the HT
