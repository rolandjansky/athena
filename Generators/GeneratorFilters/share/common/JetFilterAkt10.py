## Truth jet filter setup for anti-kT R=1.0 truth jets

include("GeneratorFilters/AntiKt10TruthJets.py")

include("GeneratorFilters/JetFilter_Fragment.py")
filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt10TruthJets"
