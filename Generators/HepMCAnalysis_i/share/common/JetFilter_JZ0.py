## Truth jet filter config for JZ0
#include("MC12JobOptions/JetFilter_JZX_Fragment.py")
include("common/JetFilter_JZX_Fragment.py")

topAlg.QCDTruthJetFilter.MinPt = 0.*GeV
topAlg.QCDTruthJetFilter.MaxPt = 20.*GeV
