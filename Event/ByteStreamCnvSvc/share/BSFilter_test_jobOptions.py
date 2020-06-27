#**************************************************************
#
# jopOptions file for selecting  ByteStream events
#
#==============================================================
# Input
from ByteStreamCnvSvc import ReadByteStream

svcMgr = theApp.serviceMgr()

ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

theApp.EvtMax = 5000
EventSelector.Input += [
    "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data",
    ]

#ByteStreamInputSvc.DumpFlag = True
#ByteStreamInputSvc.SkipNeventBeforeNext=10

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# AthenaOutputStream Name
OutStreamName="OutputStreamBSCopy"

# make a prescale algorithm
#from PrimaryDPDMaker.PrescaleFilter import PrescaleFilter
from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
Prescaler = PrimaryDPDPrescaler( OutStreamName + "_Prescaler", Prescale=2 )
topSequence += Prescaler

# BS OutputStream Tool 
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc,ByteStreamOutputStreamCopyTool
bsCopyTool = ByteStreamOutputStreamCopyTool("OutputStreamBSCopyTool")
svcMgr.ToolSvc += bsCopyTool

# BS output Svc 
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc",
                                            OutputDirectory=".",
                                            SimpleFileName="PrescaledEvents.data"
                                            )
svcMgr += bsOutputSvc 

# Set BS OutputSvc/InputSvc  to BS OutputStream Tool 
bsCopyTool.ByteStreamOutputSvc=bsOutputSvc
bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

# create AthenaOutputStream for BS Copy 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
OutputStreamBSCopy = AthenaOutputStream( OutStreamName, WritingTool=bsCopyTool )
topSequence += OutputStreamBSCopy 
        
# apply prescale to the OutputStream 
OutputStreamBSCopy.AcceptAlgs =[Prescaler.getName()] 

MessageSvc.OutputLevel = DEBUG
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
