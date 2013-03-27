HIESDItemList = []

from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.withHijingEventPars:
    include ("HIRecExample/HijingOutputESDList_jobOptions.py")

if jobproperties.HIRecExampleFlags.doHIGlobal:
    include ("HIGlobal/HIGlobalOutputESDList_jobOptions.py")

if jobproperties.HIRecExampleFlags.doHIJetRec:
    include ("HIJetRec/HIJetRecOutputESDList_jobOptions.py")
