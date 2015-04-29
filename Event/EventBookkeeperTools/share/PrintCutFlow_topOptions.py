#######################################################
## Minimal athena options to execute the CutFlowSvc  ##
#######################################################

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
if 'inputFiles' in dir():
    athenaCommonFlags.FilesInput=inputFiles.split(',')
    del inputFiles

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections=athenaCommonFlags.FilesInput()

if not 'stream' in dir():
    from RecExConfig.InputFilePeeker import inputFileSummary
    try:
        stream=inputFileSummary['stream_names'][0]
    except:
        print "WARNING Unable to determine input stream name. Will Print all streams."
        stream='any'

## if not 'cycle' in dir():
##     from RecExConfig.InputFilePeeker import inputFileSummary
##     from RecExConfig.RecoFunctions import GetSkimCycle
##     cycle=GetSkimCycle(inputFileSummary)

from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc
svcMgr+=CutFlowSvc()
theApp.CreateSvc+=['CutFlowSvc']


svcMgr.CutFlowSvc.OutputLevel=INFO
#svcMgr.CutFlowSvc.OutputLevel=DEBUG
if 'cycle' in dir():
    svcMgr.CutFlowSvc.SkimmingCycle=cycle
    pass
if 'virtual' in dir():
    svcMgr.CutFlowSvc.printVirtualFilters=virtual
    pass

svcMgr.MessageSvc.defaultLimit = 9999999
