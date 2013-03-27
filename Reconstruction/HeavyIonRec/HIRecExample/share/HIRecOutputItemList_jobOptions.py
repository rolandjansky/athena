from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.doHIGlobal:    
    include ("HIGlobal/HIGlobalOutputItemList_jobOptions.py")

if jobproperties.HIRecExampleFlags.doHIJetRec:
    include ("HIJetRec/HIJetRecOutputItemList_jobOptions.py")

