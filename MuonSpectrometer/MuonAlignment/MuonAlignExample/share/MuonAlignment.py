import os

from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import ToolSvc

if align.readEventList() :

    from TrkAlignGenAlgs.SelectEventNumberGetter import SelectEventNumberGetter
    selectEvtNumber = SelectEventNumberGetter().SelectEventNumberHandle()
    #selectEvtNumber.EventList = "goodEvts_"+align.fileRootName()+"_iter0.txt"
    #selectEvtNumber.EventList = "goodEvts_iter0.txt"
    selectEvtNumber.EventList = align.eventList()
    #selectEvtNumber.EventList = "goodEvts.txt"

if 'errorScaleDb' in dir() and errorScaleDb!='':
    
    errScaleDbName=errorScaleDb
    errScaleDbNameSplit=errScaleDbName.rsplit("/",1)
    if len(errScaleDbNameSplit)>1:
        errScaleDbName=errScaleDbNameSplit[1]
    from IOVDbSvc.CondDB import conddb
    IOVDbSvc.dbConnection="sqlite://X;schema=mycool.db;dbname=OFLP200"
    conddb.addFolder("","<dbConnection>sqlite://X;schema=mycool.db;dbname=OFLP200</dbConnection> /MUON/TrkErrorScaling" + "<tag>MuonTrkErrorScaling_nominal</tag>", force=True)
    conddb.addFolderWithTag(errScaleDbName,'/MUON/TrkErrorScaling','MuonTrkErrorScaling_nominal')
    
    from MuonErrorScalingTools.MuonErrorScalingToolsConf import Muon__MuonRIO_OnTrackErrorScalingTool,Muon__MuonErrorScaleDbTool
    muonErrorScaleDbTool = Muon__MuonErrorScaleDbTool("MuonErrorScaleDbTool",
                                                      OutputLevel=align.OutputLevel())
    ToolSvc += muonErrorScaleDbTool
    muonErrorScalingTool = Muon__MuonRIO_OnTrackErrorScalingTool("MuonRIO_OnTrackErrorScalingTool",
                                                                 OutputLevel=align.OutputLevel(),
                                                                 ErrorScaleDbTool=muonErrorScaleDbTool)
    ToolSvc += muonErrorScalingTool

    from MuonRecExample import MuonRecTools
    MdtDriftCircleOnTrackCreator  = MuonRecTools.getPublicTool("MdtDriftCircleOnTrackCreator")
    MdtDriftCircleOnTrackCreator.DoErrorScaling = True
    MdtDriftCircleOnTrackCreator.OutputLevel = align.OutputLevel()
    MdtDriftCircleOnTrackCreator.ErrorScalingTool=muonErrorScalingTool
    
    CscClusterOnTrackCreator = MuonRecTools.getPublicTool("CscClusterOnTrackCreator")
    CscClusterOnTrackCreator.DoErrorScaling = True
    CscClusterOnTrackCreator.OutputLevel = align.OutputLevel()
    CscClusterOnTrackCreator.ErrorScalingTool=muonErrorScalingTool
    
    MuonClusterOnTrackCreator = MuonRecTools.getPublicTool("MuonClusterOnTrackCreator")
    MuonClusterOnTrackCreator.DoErrorScaling = True
    MuonClusterOnTrackCreator.OutputLevel = align.OutputLevel()
    MuonClusterOnTrackCreator.ErrorScalingTool=muonErrorScalingTool

try:
    doSetup
    doSetup()
except:
    print "user defined setup method does not exist"


outputPath = align.outputPath()
if outputPath and not os.path.isdir(outputPath) :
    print "creating new directory ",outputPath
    os.mkdir(outputPath)

#from MuonCompetingClustersOnTrackCreator.MuonCompetingClustersOnTrackCreatorConf import Muon__TriggerChamberClusterOnTrackCreator
#ToolSvc+=Muon__TriggerChamberClusterOnTrackCreator(OutputLevel=VERBOSE)

#ToolSvc.DCMathSegmentMaker.MuonCompetingClustersCreator.OutputLevel=DEBUG

ToolSvc.MuonClusterOnTrackCreator.FixedErrorTgcEta = 80.

#print ToolSvc.DCMathSegmentMaker.MuonCompetingClustersCreator.OutputLevel
#print ToolSvc

#from AthenaCommon.DetFlags import DetFlags
#DetFlags.CSC_setOff()
DetFlags.Print()

#MessageSvc=Service('MessageSvc')
#MessageSvc.OutputLevel = DEBUG

geoModelSvc=Service('GeoModelSvc')
geoModelSvc.IgnoreTagDifference=True

#ToolSvc.MGM_AlignmentDbTool.DumpALines = True

if align.doVP1():
  from VP1Algs.VP1AlgsConf import VP1Alg
  topSequence += VP1Alg()

print topSequence
print svcMgr
