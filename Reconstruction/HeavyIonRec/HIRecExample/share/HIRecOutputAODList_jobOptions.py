HIAODItemList = []

from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.doHIGlobal:
    from HIGlobal.HIGlobalUtils import AppendOutputList
    AppendOutputList(HIAODItemList)


if jobproperties.HIRecExampleFlags.doHIJetRec:
    from HIJetRec.HIJetRecUtils import AppendOutputList
    AppendOutputList(HIAODItemList)


if DetFlags.detdescr.ZDC_on():
    from ZdcRec.ZdcRecUtils import AppendOutputList
    AppendOutputList(HIAODItemList)
