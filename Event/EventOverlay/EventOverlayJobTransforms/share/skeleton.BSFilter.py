from AthenaCommon.Logging import logging
BSFilterLog = logging.getLogger('BSFilter')
BSFilterLog.info( '****************** STARTING BSFilter *****************' )

BSFilterLog.info( '**** Transformation run arguments' )
BSFilterLog.info( str(runArgs) )

#---------------------------
# Job definition parameters:
theApp.EvtMax           = runArgs.maxEvents

if not hasattr( runArgs, "OutputBSFilterFile"):
    raise RuntimeError ("No output BS file defined")

#---------------------------
BSFilterLog.info( '**** ByteStreamInputSvc configuration' )

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr = theApp.serviceMgr()
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc
#theApp.SkipEvents = runArgs.skipEvents #doesn't work
MessageSvc.OutputLevel = INFO
ByteStreamInputSvc.FullFileName = runArgs.inputBSFile
#ByteStreamInputSvc.ValidateEvent=False
#ByteStreamInputSvc.DumpFlag = True
#ByteStreamInputSvc.SkipNeventBeforeNext=10
print ByteStreamInputSvc

# ---------------------------
# Service to write out BS events
BSFilterLog.info( '**** ByteStreamOutputSvc configuration' )

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
if runArgs.OutputBSFilterFile.startswith("simple"):
    bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc0",OutputDirectory="./",SimpleFileName=runArgs.OutputBSFilterFile)
else:
    bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc0",MaxFileNE=100,OutputDirectory="./",ProjectTag=runArgs.OutputBSFilterFile,AppName="Filtered",FileTag="test")
svcMgr += bsOutputSvc

print bsOutputSvc

# ---------------------------
BSFilterLog.info( '**** ByteStreamFilter configuration' )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get the filter algortihm
from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import CTPByteStreamTool,RecCTPByteStreamTool
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["ROIB::RoIBResult/RoIBResult", "MuCTPI_RDO/MUCTPI_RDO", "CTP_RDO/CTP_RDO", "MuCTPI_RIO/MUCTPI_RIO", "CTP_RIO/CTP_RIO"
]
from OverlayCommonAlgs.OverlayCommonAlgsConf import  BSFilter
filAlg=BSFilter("BSFilter")
topSequence+=filAlg
if hasattr( runArgs, "TriggerBit"):
    filAlg.TriggerBit = runArgs.TriggerBit
else:
    filAlg.TriggerBit = -1

#    if hasattr( runArgs, "EventIdFile"):
if hasattr( runArgs, "outputTXT_EVENTIDFile"):
    filAlg.EventIdFile=runArgs.outputTXT_EVENTIDFile # The name of the file to write to for EventIdModifierSvc lines
else:
    filAlg.EventIdFile=""

if hasattr( runArgs, "FilterFile"):
    filAlg.filterfile=runArgs.FilterFile # The thing made from the TAG files via "root -l -b -q HITAGprinter_run.C"
else:
    filAlg.filterfile = ""

# ---------------------------
BSFilterLog.info( '**** ByteStreamCopyTool configuration' )

# BS OutputStream Tool
OutStreamName="OutputStreamBSCopy"
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamOutputStreamCopyTool
bsCopyTool = ByteStreamOutputStreamCopyTool("OutputStreamBSCopyTool")
svcMgr.ToolSvc += bsCopyTool

bsCopyTool.ByteStreamOutputSvc=bsOutputSvc
bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

# ---------------------------
BSFilterLog.info( '**** AthenaOutputStream configuration' )

# create AthenaOutputStream for BS Copy 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
OutputStreamBSCopy = AthenaOutputStream( OutStreamName, WritingTool=bsCopyTool )
topSequence += OutputStreamBSCopy
OutputStreamBSCopy.AcceptAlgs =["BSFilter"] 

print topSequence

#---------------------------
# Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)
