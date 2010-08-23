include.block("TrigMissingETHypo/TriggerConfig_MET.py")

#print "METSlice Flags:"
TriggerFlags.METSlice.printFlags("MET Slice Flags")

from TrigMissingETHypo.MissingET import MissingET
te150 = MissingET("te150")
te250 = MissingET("te250")
te360 = MissingET("te360")
te650 = MissingET("te650")
xe15 = MissingET("xe15")
xe20 = MissingET("xe20")
xe25 = MissingET("xe25")
xe30 = MissingET("xe30")
xe40 = MissingET("xe40")
xe50 = MissingET("xe50")
xe70 = MissingET("xe70")
xe80 = MissingET("xe80")

#print deprecated?
#print "MissingETSlice Flags:" 
#TriggerFlags.METSlice.printFlags(" Missing ET Slice Flags ")

include("TrigMissingETHypo/TriggerConfig_MET_Level1.py")
if triggerMenu.signatureIsEnabled("te150"):
    te150.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("te250"):
    te250.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("te360"):
    te360.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("te650"):
    te650.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe15"):
    xe15.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe20"):
    xe20.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe25"):
    xe25.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe30"):
    xe30.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe40"):
    xe40.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe50"):
    xe50.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe70"):
    xe70.generateMenu(triggerPythonConfig)

if triggerMenu.signatureIsEnabled("xe80"):
    xe80.generateMenu(triggerPythonConfig)

