HIESDItemList = []

from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.doHIGlobal:
    from HIGlobal.HIGlobalUtils import AppendOutputList
    AppendOutputList(HIESDItemList)

if jobproperties.HIRecExampleFlags.doHIJetRec:
    from HIJetRec.HIJetRecUtils import AppendOutputList
    AppendOutputList(HIESDItemList)
