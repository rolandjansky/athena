# Andrzej Olszewski
# 13 March 2008
###################

# debug print at the end of setup
DetFlags.Print()

# Andrzej Olszewski:  mods for including HijingEventParams
# --------------------------------------------------------
# Until HijingOutputAODList_jobOptions.py is not used

from HIRecExample.HIRecExampleFlags import jobproperties

if not jobproperties.HIRecExampleFlags.ppMode :
  print "Applying HI AOD postOptions mods"

  if 'doTruth' in dir() and doTruth:
    if "TruthExamples" not in theApp.Dlls:
        theApp.Dlls += [ "TruthExamples" ]
    if "PrintHijingPars" not in theApp.TopAlg:
        theApp.TopAlg += ["PrintHijingPars"]

  if 'StreamAOD' in dir():
       StreamAOD.ItemList  += ["54790518#Hijing_event_params"]
       print StreamAOD.ItemList

#=========================================================
#
# End of job options file
#
###############################################################
