
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr = theApp.serviceMgr()
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

theApp.EvtMax = -1
theApp.SkipEvents = 0
#MessageSvc.OutputLevel = DEBUG
#MessageSvc.debugLimit = 10000

###############
#make a filelist of local files
filelist = []

#dq2-ls -f -p -H data10_7TeV.00167776.physics_ZeroBias.merge.RAW | sed "s%srm://osgserv04.slac.stanford.edu:8443/srm/v2/server?SFN=/xrootd/atlas/%filelist += [\"root://atl-xrdr//atlas/xrootd/%g" | sed "s%$%\"]%g" | grep xrootd > ./filelist.txt
#include("./filelist.txt")

mydir = "/u/at/ahaas/nfs3/"
mydir+="user.haastyle.data10_hi.00170467.physics_bulk.daq.RAW_der1317833222/"
files = os.listdir(mydir)
for f in files:
    filelist += [mydir+f]

ByteStreamInputSvc.FullFileName += filelist
################

#use the FileStager
#sampleFile="samples/"+RUN+".physics_ZeroBias.merge.RAW.def"
#include ("EventOverlayJobTransforms/input_FileStager.py")
#ByteStreamInputSvc.FullFileName = ic

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

from OverlayCommonAlgs.OverlayCommonAlgsConf import  BSFilter, ByteStreamMultipleOutputStreamCopyTool
filAlg=BSFilter("BSFilter")
filAlg.TriggerBit=BIT # The trigger bit to select - i.e. don't select by trigger bit, use TAG info!
filAlg.filterfile=FILTERFILE # The thing made from the TAG files via "root -l -b -q HITAGprinter_run.C"
print filAlg
topSequence+=filAlg

# BS OutputStream Tool
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
bsCopyTool = ByteStreamMultipleOutputStreamCopyTool("MultipleOutputStreamBSCopyTool")
bsCopyTool.uselbnmap=0 #don't try to read the lbn map!
svcMgr.ToolSvc += bsCopyTool

bsCopyTool.NoutputSvc = 1 #just one output file! It is set to have a max of 100 events, so you'll end up with more files
for i in range(0,bsCopyTool.NoutputSvc):
    bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc"+str(i),MaxFileNE=100,OutputDirectory=str(OUTPUTDIR),ProjectTag="HIevents",AppName="Filtered",FileTag="test")
    svcMgr += bsOutputSvc
    if i==0: bsCopyTool.ByteStreamOutputSvc0=bsOutputSvc
    if i==1: bsCopyTool.ByteStreamOutputSvc1=bsOutputSvc

# BS InputStream
bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

# create AthenaOutputStream for BS Copy 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
OutputStreamBSCopy = AthenaOutputStream( "OutputStreamBSCopy", WritingTool=bsCopyTool )
topSequence += OutputStreamBSCopy 
        
# apply prescale to the OutputStream 
OutputStreamBSCopy.AcceptAlgs =["BSFilter"] 

#
print bsCopyTool
