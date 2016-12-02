from AthenaCommon.Logging import logging
BS_multipleSelectorLog = logging.getLogger('BS_multipleSelector')
BS_multipleSelectorLog.info( '****************** STARTING BS_multipleSelector 2*****************' )

# Job definition parameters:
EvtMax = runArgs.maxEvents
SkipEvents = runArgs.skipEvents
BSInput = runArgs.inputBSFile
TriggerBit = runArgs.TriggerBit
Noutputs = runArgs.Noutputs
Uniq = runArgs.uniq

BS_multipleSelectorLog.info( '**** Transformation run arguments' )
BS_multipleSelectorLog.info( str(runArgs) )

maxeventsvec= eval('['+runArgs.maxeventsvec+']')
skipeventsvec= eval('['+runArgs.skipeventsvec+']')
noutputsvec= eval('['+runArgs.noutputsvec+']')
streamvec= eval('['+runArgs.streamvec+']')

if len(streamvec)>0:
    mystream=int(BSInput[0].split(".")[7].replace("stream",""))
    print "mystream is ",mystream
    mystreamindex= int(streamvec.index(mystream))
    print "mystreamindex is ",mystreamindex," for mystream ",mystream

    EvtMax=noutputsvec[mystreamindex]
    SkipEvents=skipeventsvec[mystreamindex]
    Noutputs=noutputsvec[mystreamindex]
    maxnout=max(noutputsvec)
    if maxnout>Noutputs:
        print "WARNING - settting Noutputs "+str(Noutputs)+" to maxnout ",maxnout
        Noutputs=maxnout
    print "evtmax, skip, nout = ",EvtMax,SkipEvents,Noutputs


#---------------------------
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr = theApp.serviceMgr()
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc
theApp.EvtMax = EvtMax
#theApp.SkipEvents = SkipEvents #doesn't work
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 10000;
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
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["ROIB::RoIBResult/RoIBResult",  "HLT::HLTResult/HLTResult_EF","HLT::HLTResult/HLTResult_L2","HLT::HLTResult/HLTResult_HLT", "CTP_RDO/CTP_RDO", "CTP_RIO/CTP_RIO"]

from OverlayCommonAlgs.OverlayCommonAlgsConf import  BSFilter, ByteStreamMultipleOutputStreamCopyTool
filAlg=BSFilter("BSFilter")
filAlg.TriggerBit=TriggerBit # The trigger bit to select
filAlg.EventIdFile=runArgs.EventIdFile # Not really used, but writes out the EventIdOverrides
topSequence+=filAlg
print filAlg

# BS MultipleOutputStreamTool
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
bsCopyTool = ByteStreamMultipleOutputStreamCopyTool("MultipleOutputStreamBSCopyTool")
svcMgr.ToolSvc += bsCopyTool
bsCopyTool.lbn_map_file = runArgs.LbnMapFile
bsCopyTool.skipevents = SkipEvents
bsCopyTool.trigfile = runArgs.TrigFile
bsCopyTool.NoutputSvc = Noutputs
for i in range(0,bsCopyTool.NoutputSvc):
    if len(streamvec)>0:
        bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc"+str(i),OutputDirectory="./",SimpleFileName="Zerobias"+Uniq+".bit"+str(TriggerBit)+".out"+str(i)+".RAW")
    else:
        bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc"+str(i),OutputDirectory="./",SimpleFileName="Zerobias"+Uniq+".bit"+str(TriggerBit)+".stream"+str(i)+".RAW")
        #bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc"+str(i),OutputDirectory="./",SimpleFileName=BSOutput+"_"+str(TriggerBit)+"_"+str(i)+".RAW")
    
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

#---------------------------
# Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)
