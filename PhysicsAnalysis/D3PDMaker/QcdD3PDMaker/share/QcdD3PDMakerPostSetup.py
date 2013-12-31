if primDPD.MakePrimaryDPD():
  from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
  from PrimaryDPDMaker.PrimaryDPDFlags import listESDtoDPD,listAODtoDPD

  for stream  in listESDtoDPD + listAODtoDPD:
    if MSMgr.StreamExists(stream):
      StreamESD=MSMgr.GetStream(stream)
      StreamESD.AddItem(["JetCollection#*"])
      StreamESD.AddItem(["MissingET#*"])
