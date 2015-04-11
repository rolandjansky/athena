HIAODItemList = []

if jobproperties.HIRecExampleFlags.doHIGlobal:
    from HIGlobal.HIGlobalUtils import AppendOutputList
    AppendOutputList(HIAODItemList)


if jobproperties.HIRecExampleFlags.doHIJetRec:
    from HIJetRec.HIJetRecUtils import AppendOutputList
    AppendOutputList(HIAODItemList)
