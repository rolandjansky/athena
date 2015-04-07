from AthenaCommon.Logging import logging
BSFilterLog = logging.getLogger('BSFilter')
BSFilterLog.info( '****************** STARTING BSFilter *****************' )

# BSFilterLog.info( '**** Transformation run arguments' )
# BSFilterLog.info( str(runArgs) )

#---------------------------
theApp.EvtMax = runArgs.maxEvents

#---------------------------
## Run performance monitoring (memory logging)
from PerfMonComps.PerfMonFlags import jobproperties as perfmonjp
perfmonjp.PerfMonFlags.doMonitoring = True # to enable monitoring
perfmonjp.PerfMonFlags.doFastMon = True    # to only enable a lightweight monitoring
#perfmonjp.PerfMonFlags.doSemiDetailedMonitoring = True

#---------------------------
BSFilterLog.info( '**** ByteStreamInputSvc configuration' )

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc
# ByteStreamInputSvc.FullFileName = open(runArgs.InputFileMapFile).readline().rstrip().split(',')
ByteStreamInputSvc.FullFileName = runArgs.inputBSFile

print ByteStreamInputSvc

# ---------------------------
# Service to write out BS events
BSFilterLog.info( '**** ByteStreamOutputSvc configuration' )

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc",OutputDirectory='./',SimpleFileName=runArgs.outputBS_SKIMFile)
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
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["ROIB::RoIBResult/RoIBResult", "MuCTPI_RDO/MUCTPI_RDO", "CTP_RDO/CTP_RDO", "MuCTPI_RIO/MUCTPI_RIO", "CTP_RIO/CTP_RIO"]

from OverlayCommonAlgs.OverlayCommonAlgsConf import  BSFilter
filAlg = BSFilter("BSFilter")
topSequence += filAlg
if hasattr( runArgs, "triggerBit"):
    filAlg.TriggerBit = runArgs.triggerBit
else:
    filAlg.TriggerBit = -1

if hasattr(runArgs, 'eventIdFile'):
    filAlg.EventIdFile=runArgs.eventIdFile # The name of the file to write to for EventIdModifierSvc lines
else:
    filAlg.EventIdFile=""

if hasattr( runArgs, "FilterFile"): #TODO currently no such argument
    filAlg.filterfile=runArgs.FilterFile # The thing made from the TAG files via "root -l -b -q HITAGprinter_run.C"
else:
    filAlg.filterfile = ""

# ---------------------------
BSFilterLog.info( '**** ByteStreamCopyTool configuration' )

if hasattr( runArgs, 'InputLbnMapFile'):
    from OverlayCommonAlgs.OverlayCommonAlgsConf import ByteStreamMultipleOutputStreamCopyTool
    bsCopyTool = ByteStreamMultipleOutputStreamCopyTool("MultipleOutputStreamBSCopyTool")
    bsCopyTool.lbn_map_file = runArgs.InputLbnMapFile
    bsCopyTool.NoutputSvc = 1
    bsCopyTool.ByteStreamOutputSvc0=bsOutputSvc
else:
    from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamOutputStreamCopyTool
    bsCopyTool = ByteStreamOutputStreamCopyTool("OutputStreamBSCopyTool")
    bsCopyTool.ByteStreamOutputSvc=bsOutputSvc

svcMgr.ToolSvc += bsCopyTool

bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

# ---------------------------
BSFilterLog.info( '**** AthenaOutputStream configuration' )

# create AthenaOutputStream for BS Copy 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
OutputStreamBSCopy = AthenaOutputStream( "OutputStreamBSCopy", WritingTool=bsCopyTool )
topSequence += OutputStreamBSCopy

OutputStreamBSCopy.AcceptAlgs =["BSFilter"] 

print topSequence

# ---------------------------
# Post-include

if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)
