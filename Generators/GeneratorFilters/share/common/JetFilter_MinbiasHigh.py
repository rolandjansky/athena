## Min bias truth jet filter for min bias sample preparation (high slice)

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")

include("GeneratorFilters/JetFilter_Fragment.py")
filtSeq.QCDTruthJetFilter.MinPt = 35.*GeV
filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt6TruthJets"
filtSeq.QCDTruthJetFilter.DoShape = False
