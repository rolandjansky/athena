HIAODItemList = []

from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.withHijingEventPars :
    include ("HIRecExample/HijingOutputAODList_jobOptions.py")

if jobproperties.HIRecExampleFlags.doHIGlobal:
    include ("HIGlobal/HIGlobalOutputAODList_jobOptions.py")

print "HIJetRec AOD objects turned off due to bug!!!"
print "Defining objects results in reconstruction run again!!!"
#if jobproperties.HIRecExampleFlags.doHIJetRec:
#    include ("HIJetRec/HIJetRecOutputAODList_jobOptions.py")
