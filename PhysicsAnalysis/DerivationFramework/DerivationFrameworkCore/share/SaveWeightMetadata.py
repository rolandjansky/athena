
from AthenaCommon.GlobalFlags import globalflags

if globalflags.DataSource() == 'geant4':
    from PyUtils.MetaReader import read_metadata
    input_file = svcMgr.EventSelector.InputCollections[0]
    metadata = read_metadata(input_file)
    metadata = metadata[input_file]  # promote all keys one level up

    inputStreamName = 'unknownStream'
    for class_name, name in metadata['metadata_items'].items():
        if name == 'EventStreamInfo':
            inputStreamName = class_name
            break

    name = "PDFSumOfWeights"
    cutflowsvc = CfgMgr.CutFlowSvc("CutFlowSvc"+name)
    cutflowsvc.InputStream = inputStreamName
    cutflowsvc.OutputCollName=name
    cutflowsvc.OutputIncompleteCollName = "Incomplete"+name

    svcMgr += cutflowsvc

    theApp.CreateSvc += ["CutFlowSvc/CutFlowSvc"+name]

