
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr = theApp.serviceMgr()
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

theApp.EvtMax = -1
theApp.SkipEvents = 0
#MessageSvc.OutputLevel = DEBUG
#MessageSvc.debugLimit = 10000

#make a filelist of local files
#dq2-ls -f -p -H data10_7TeV.00167776.physics_ZeroBias.merge.RAW | sed "s%srm://osgserv04.slac.stanford.edu:8443/srm/v2/server?SFN=/xrootd/atlas/%filelist += [\"root://atl-xrdr//atlas/xrootd/%g" | sed "s%$%\"]%g" | grep xrootd > ./filelist.txt
#filelist = []
#include("./filelist.txt")
#ByteStreamInputSvc.FullFileName += filelist

#use the FileStager
sampleFile="samples/"+RUN+".physics_ZeroBias.merge.RAW.def"
include ("EventOverlayJobTransforms/input_FileStager.py")
ByteStreamInputSvc.FullFileName = ic

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
filAlg.TriggerBit=BIT # The trigger bit to select
print filAlg
topSequence+=filAlg

# BS OutputStream Tool
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
bsCopyTool = ByteStreamMultipleOutputStreamCopyTool("MultipleOutputStreamBSCopyTool")
svcMgr.ToolSvc += bsCopyTool

bsCopyTool.NoutputSvc = 50
for i in range(0,bsCopyTool.NoutputSvc):
    bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc"+str(i),OutputDirectory="./",SimpleFileName="PrescaledEvents_bit"+str(BIT)+"_"+str(i)+".RAW")
    svcMgr += bsOutputSvc
    if i==0: bsCopyTool.ByteStreamOutputSvc0=bsOutputSvc
    if i==1: bsCopyTool.ByteStreamOutputSvc1=bsOutputSvc
    if i==2: bsCopyTool.ByteStreamOutputSvc2=bsOutputSvc
    if i==3: bsCopyTool.ByteStreamOutputSvc3=bsOutputSvc
    if i==4: bsCopyTool.ByteStreamOutputSvc4=bsOutputSvc
    if i==5: bsCopyTool.ByteStreamOutputSvc5=bsOutputSvc
    if i==6: bsCopyTool.ByteStreamOutputSvc6=bsOutputSvc
    if i==7: bsCopyTool.ByteStreamOutputSvc7=bsOutputSvc
    if i==8: bsCopyTool.ByteStreamOutputSvc8=bsOutputSvc
    if i==9: bsCopyTool.ByteStreamOutputSvc9=bsOutputSvc
    if i==10: bsCopyTool.ByteStreamOutputSvc10=bsOutputSvc
    if i==11: bsCopyTool.ByteStreamOutputSvc11=bsOutputSvc
    if i==12: bsCopyTool.ByteStreamOutputSvc12=bsOutputSvc
    if i==13: bsCopyTool.ByteStreamOutputSvc13=bsOutputSvc
    if i==14: bsCopyTool.ByteStreamOutputSvc14=bsOutputSvc
    if i==15: bsCopyTool.ByteStreamOutputSvc15=bsOutputSvc
    if i==16: bsCopyTool.ByteStreamOutputSvc16=bsOutputSvc
    if i==17: bsCopyTool.ByteStreamOutputSvc17=bsOutputSvc
    if i==18: bsCopyTool.ByteStreamOutputSvc18=bsOutputSvc
    if i==19: bsCopyTool.ByteStreamOutputSvc19=bsOutputSvc
    if i==20: bsCopyTool.ByteStreamOutputSvc20=bsOutputSvc
    if i==21: bsCopyTool.ByteStreamOutputSvc21=bsOutputSvc
    if i==22: bsCopyTool.ByteStreamOutputSvc22=bsOutputSvc
    if i==23: bsCopyTool.ByteStreamOutputSvc23=bsOutputSvc
    if i==24: bsCopyTool.ByteStreamOutputSvc24=bsOutputSvc
    if i==25: bsCopyTool.ByteStreamOutputSvc25=bsOutputSvc
    if i==26: bsCopyTool.ByteStreamOutputSvc26=bsOutputSvc
    if i==27: bsCopyTool.ByteStreamOutputSvc27=bsOutputSvc
    if i==28: bsCopyTool.ByteStreamOutputSvc28=bsOutputSvc
    if i==29: bsCopyTool.ByteStreamOutputSvc29=bsOutputSvc
    if i==30: bsCopyTool.ByteStreamOutputSvc30=bsOutputSvc
    if i==31: bsCopyTool.ByteStreamOutputSvc31=bsOutputSvc
    if i==32: bsCopyTool.ByteStreamOutputSvc32=bsOutputSvc
    if i==33: bsCopyTool.ByteStreamOutputSvc33=bsOutputSvc
    if i==34: bsCopyTool.ByteStreamOutputSvc34=bsOutputSvc
    if i==35: bsCopyTool.ByteStreamOutputSvc35=bsOutputSvc
    if i==36: bsCopyTool.ByteStreamOutputSvc36=bsOutputSvc
    if i==37: bsCopyTool.ByteStreamOutputSvc37=bsOutputSvc
    if i==38: bsCopyTool.ByteStreamOutputSvc38=bsOutputSvc
    if i==39: bsCopyTool.ByteStreamOutputSvc39=bsOutputSvc
    if i==40: bsCopyTool.ByteStreamOutputSvc40=bsOutputSvc
    if i==41: bsCopyTool.ByteStreamOutputSvc41=bsOutputSvc
    if i==42: bsCopyTool.ByteStreamOutputSvc42=bsOutputSvc
    if i==43: bsCopyTool.ByteStreamOutputSvc43=bsOutputSvc
    if i==44: bsCopyTool.ByteStreamOutputSvc44=bsOutputSvc
    if i==45: bsCopyTool.ByteStreamOutputSvc45=bsOutputSvc
    if i==46: bsCopyTool.ByteStreamOutputSvc46=bsOutputSvc
    if i==47: bsCopyTool.ByteStreamOutputSvc47=bsOutputSvc
    if i==48: bsCopyTool.ByteStreamOutputSvc48=bsOutputSvc
    if i==49: bsCopyTool.ByteStreamOutputSvc49=bsOutputSvc

# BS InputStream
bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

# create AthenaOutputStream for BS Copy 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
OutputStreamBSCopy = AthenaOutputStream( "OutputStreamBSCopy", WritingTool=bsCopyTool )
topSequence += OutputStreamBSCopy 
        
# apply prescale to the OutputStream 
OutputStreamBSCopy.AcceptAlgs =["BSFilter"] 

