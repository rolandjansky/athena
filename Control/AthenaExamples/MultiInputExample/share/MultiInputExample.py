include( "AthenaCommon/AthenaCommonFlags.py" )

from AthenaCommon.AppMgr import ServiceMgr

include( "MultiInputExample/MultiInputInit.py" )

inp1=inputObject();
inp1.inputCollections=["SimplePoolFile1.root"];
inp1.objectName="Input1"
inputObjectList += [inp1];

inp2=inputObject();
inp2.inputCollections=["SimplePoolFile2.root"];
inp2.objectName="Input2"
inputObjectList += [inp2];

pileUpEventLoopMgr = PileUpEventLoopMgr();
pileUpEventLoopMgr.OutStreamType = "AthenaOutputStream";
ServiceMgr += pileUpEventLoopMgr

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = 3

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 5
SkipEvents = 4


#Init input streams
initMultiInput()

#load relevant libraries
theApp.Dlls += [ "MultiInputExample" ]

#top algorithms to be run
theApp.TopAlg = [ "MyMultiInputAlg" ]
MyMultiInputAlg = Algorithm( "MyMultiInputAlg" )
