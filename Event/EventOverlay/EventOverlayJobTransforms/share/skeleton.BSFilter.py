from AthenaCommon.Logging import logging
BSFilterLog = logging.getLogger('BSFilter')
BSFilterLog.info( '****************** STARTING BSFilter *****************' )

# Job definition parameters:
EvtMax           = runArgs.maxEvents
BSInput     = runArgs.inputBSFile
BSOutput  = runArgs.OutputBSFilterFile
BSOutputDir  = "./" #runArgs.OutputBSFilterFileDir
TriggerBit = runArgs.TriggerBit

BSFilterLog.info( '**** Transformation run arguments' )
BSFilterLog.info( str(runArgs) )

#---------------------------
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr = theApp.serviceMgr()
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc
theApp.EvtMax = EvtMax
#theApp.SkipEvents = runArgs.skipEvents #doesn't work
MessageSvc.OutputLevel = INFO
ByteStreamInputSvc.FullFileName = BSInput
#ByteStreamInputSvc.ValidateEvent=False
#ByteStreamInputSvc.DumpFlag = True
#ByteStreamInputSvc.SkipNeventBeforeNext=10
print ByteStreamInputSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get the filter algortihm
from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import CTPByteStreamTool,RecCTPByteStreamTool
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "ROIB::RoIBResult/RoIBResult",
    "MuCTPI_RDO/MUCTPI_RDO",
    "CTP_RDO/CTP_RDO",
    "MuCTPI_RIO/MUCTPI_RIO",
    "CTP_RIO/CTP_RIO"
    ]
from OverlayCommonAlgs.OverlayCommonAlgsConf import  BSFilter
filAlg=BSFilter("BSFilter")
filAlg.TriggerBit=TriggerBit # The trigger bit to select
filAlg.EventIdFile=runArgs.EventIdFile # The name of the file to write to for EventIdModifierSvc lines
filAlg.filterfile=runArgs.FilterFile # The thing made from the TAG files via "root -l -b -q HITAGprinter_run.C"
topSequence+=filAlg

# BS OutputStream Tool
OutStreamName="OutputStreamBSCopy"
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc,ByteStreamOutputStreamCopyTool
bsCopyTool = ByteStreamOutputStreamCopyTool("OutputStreamBSCopyTool")
svcMgr.ToolSvc += bsCopyTool

# Service to write out BS events
if BSOutput.startswith("simple"):
    bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc0",OutputDirectory=BSOutputDir,SimpleFileName=BSOutput)
else:
    bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc0",MaxFileNE=100,OutputDirectory=BSOutputDir,ProjectTag=BSOutput,AppName="Filtered",FileTag="test")
svcMgr += bsOutputSvc
bsCopyTool.ByteStreamOutputSvc=bsOutputSvc
bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

# create AthenaOutputStream for BS Copy 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
OutputStreamBSCopy = AthenaOutputStream( OutStreamName, WritingTool=bsCopyTool )
topSequence += OutputStreamBSCopy
OutputStreamBSCopy.AcceptAlgs =["BSFilter"] 

#---------------------------
# Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)
