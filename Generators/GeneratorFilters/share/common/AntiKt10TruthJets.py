## Configure jet container for anti-kT R=1.0 truth jets                                                                                                                  

evgenConfig.findJets = True

## Turn off ghost association algorithms                                                                                                                                 
from JetRec.JetRecFlags import jetFlags
jetFlags.truthFlavorTags = []
# Turning off tracks which are otherwise on by default                                                                                                                   
# If you wish to use tracks, simply add the following two lines to you JOs BEFORE including this fragment, do not alter this fragment.                                   
# from JetRec.JetRecFlags import jetFlags                                                                                                                                
# jetFlags.useTracks.set_Value_and_Lock(True)                                                                                                                            
jetFlags.useTracks = False

from JetRec.JetRecStandard import jtm
jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth",
                 modifiersin="none",
                 ptmin=50000.)