## Truth jet filter setup for anti-kT R=0.6 truth jets

from JetRec.JetGetters import *
a4alg = make_StandardJetGetter('AntiKt', 0.6, 'Truth').jetAlgorithmHandle()

#include("MC12JobOptions/JetFilter_Fragment.py")
include("common/JetFilter_Fragment.py")
topAlg.QCDTruthJetFilter.TruthJetContainer = "AntiKt6TruthJets"
