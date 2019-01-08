## Truth jet filter setup for anti-kT R=0.4 truth jets

include("GeneratorFilters/AntiKt4TruthJets.py")

include("GeneratorFilters/JetFilter_Fragment.py")
filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt4TruthJets"
