
from AthenaCommon.GlobalFlags import globalflags

if globalflags.DataSource() == 'geant4':
    from PyUtils import AthFile
    af = AthFile.fopen( svcMgr.EventSelector.InputCollections[0] )

    from EventBookkeeperTools.CutFlowHelpers import GetInputStreamNameFromMetaDataItemList
    inputStreamName = GetInputStreamNameFromMetaDataItemList(af.fileinfos["metadata_items"] )

    name = "PDFSumOfWeights"
    cutflowsvc = CfgMgr.CutFlowSvc("CutFlowSvc"+name)
    cutflowsvc.InputStream = inputStreamName
    cutflowsvc.OutputCollName=name
    cutflowsvc.OutputIncompleteCollName = "Incomplete"+name

    svcMgr += cutflowsvc

    theApp.CreateSvc += ["CutFlowSvc/CutFlowSvc"+name]

