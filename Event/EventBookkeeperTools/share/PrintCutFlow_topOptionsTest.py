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

if not 'cycle' in dir():
    print "INFO cycle info is not yet used by CutFlowSvc::print(). Coming soon!"
    from RecExConfig.InputFilePeeker import inputFileSummary
    from RecExConfig.RecoFunctions import GetSkimCycle
    cycle=GetSkimCycle(inputFileSummary)

from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc
svcMgr+=CutFlowSvc()
theApp.CreateSvc+=['CutFlowSvc']


#Arbitrarily changing cycle into cycle+1 since in this example
# we are not using Reco_trf and all the tools which do this automatically
#cycle=cycle+1

from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()

from EventBookkeeperTools.EventBookkeeperToolsConf import myCppFilterTest
myCpp1=myCppFilterTest('myCpp1')
myCpp1.OutputLevel=INFO
myCpp1.cut1=21.
myCpp1.cut2=12.
topSequence+=myCpp1

myCpp2=myCppFilterTest('myCpp2')
myCpp2.OutputLevel=INFO
myCpp2.cut1=212.
myCpp2.cut2=122.
topSequence+=myCpp2

from EventBookkeeperTools.myTestLogicalFilterCombiner import myTestLogicalFilterCombiner
_myLFC=myTestLogicalFilterCombiner('myLFC')
_myLFC.cmdstring = "myCpp1 or myCpp2"
_myLFC.OutputLevel=INFO
topSequence+=_myLFC



svcMgr.CutFlowSvc.OutputLevel=INFO
svcMgr.CutFlowSvc.OutputLevel=DEBUG
svcMgr.CutFlowSvc.SkimmingCycle=cycle

svcMgr.MessageSvc.defaultLimit = 9999999
