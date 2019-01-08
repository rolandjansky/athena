## Configure jet container for anti-kT R=0.4 truth jets with flavour labels

## Note: this cannot be used simultaneously with a non-labelled jet container
## since jetFlags.truthFlavorTags is a global variable

evgenConfig.findJets = True

## Turn on ghost association algorithms
from JetRec.JetRecFlags import jetFlags
jetFlags.truthFlavorTags = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                            "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                            "TausFinal",
                            "Partons",
                            ]
# Turning off tracks which are otherwise on by default
# If you wish to use tracks, simply add the following two lines to you JOs BEFORE including this fragment, do not alter this fragment.
# from JetRec.JetRecFlags import jetFlags
# jetFlags.useTracks.set_Value_and_Lock(True)
jetFlags.useTracks = False

## truthpartondr    => adds cone-based label
## partontruthlabel => adds ghost-based label
## removeconstit    => empties container of jet constituents to reduce filesize
from JetRec.JetRecStandard import jtm
jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth",
                 modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit],
                 ptmin=7000.)
