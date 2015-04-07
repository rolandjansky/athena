include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr = theApp.serviceMgr()
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

theApp.EvtMax = 20
theApp.SkipEvents = 0
MessageSvc.OutputLevel = INFO

ByteStreamInputSvc.FullFileName += ["/u/at/ahaas/nfs2/temp/minbias/data10_7TeV.00152845.physics_RNDM.merge.RAW/data10_7TeV.00152845.physics_RNDM.merge.RAW._lb0200._0001.1","/u/at/ahaas/nfs2/temp/minbias/data10_7TeV.00152845.physics_RNDM.merge.RAW/data10_7TeV.00152845.physics_RNDM.merge.RAW._lb0201._0001.1"]
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
filAlg.TriggerBit=63 # The trigger bit to select
topSequence+=filAlg

# BS OutputStream Tool
OutStreamName="OutputStreamBSCopy"
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc,ByteStreamOutputStreamCopyTool
bsCopyTool = ByteStreamOutputStreamCopyTool("OutputStreamBSCopyTool")
svcMgr.ToolSvc += bsCopyTool

# Service to write out BS events
bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc0",OutputDirectory="temp/",SimpleFileName="SelectedBSEvents")
svcMgr += bsOutputSvc
bsCopyTool.ByteStreamOutputSvc=bsOutputSvc
bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

# create AthenaOutputStream for BS Copy 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
OutputStreamBSCopy = AthenaOutputStream( OutStreamName, WritingTool=bsCopyTool )
topSequence += OutputStreamBSCopy
OutputStreamBSCopy.AcceptAlgs =["BSFilter"] 

