import os
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("LArCalibProcessing/GetInputFiles.py")

#######################################################
#       Run properties
#######################################################

if not 'RunNumber' in dir():
   RunNumber = 500000
   
if not ' GainList' in dir():
    GainList = ["HIGH", "MEDIUM", "LOW"]

if not 'ChannelSelection' in dir():
   ChannelSelection=""

if not 'LArInputTag' in dir():
   LArInputTag = "-UPD3-00"

if not 'LArInputKey' in dir():
   LArInputKey = [""]

if not 'AllChannels2Ntuple' in dir():
   AllChannels2Ntuple = False

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2"

if not 'DBConnection' in dir():
   DBConnection = DBConnectionCOOL   

## Output

if not 'OutputRootFileDir' in dir():
   OutputRootFileDir  =  os.getcwd()     

if not 'BaseFileName' in dir():
   BaseFileName = "LArParams"
   
BaseFileName = BaseFileName+"_"+str(RunNumber)

if not 'OutputRootFileName' in dir():
   OutputRootFileName = BaseFileName+".root"
   
###########################################################################
#
# Global settings
#
###########################################################################

include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")


## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
svcMgr.IOVDbSvc.DBInstance=""

if conddb.isMC:
   include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
   conddb.addFolder("LAR_OFL","/LAR/BadChannels/BadChannels<tag>LArBadChannelsBadChannels-IOVDEP-06</tag>",className="CondAttrListCollection")
else:
   include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
   conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>",className="CondAttrListCollection")

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg
theLArBadChannelCondAlg=LArBadChannelCondAlg()
theLArBadChannelCondAlg.ReadKey="/LAR/BadChannels/BadChannels"
condSeq+=theLArBadChannelCondAlg


## The reference is the Oracle DB
if  'LArCaliParamsFolder' in dir():
    if not 'InputTagSpecCali' in dir():
       InputTagSpecCali = LArCalibFolderTag(LArCaliParamsFolder,LArInputTag)

    conddb.addFolder("",LArCaliParamsFolder+"<tag>"+InputTagSpecCali+"</tag><dbConnection>"+DBConnection+"</dbConnection>"+ChannelSelection,className="LArCaliPulseParamsComplete")

   
if 'LArDetParamsFolder' in dir():
    if not 'InputTagSpecDet' in dir():
       InputTagSpecDet = LArCalibFolderTag(LArDetParamsFolder,LArInputTag)

    conddb.addFolder("",LArDetParamsFolder+"<tag>"+InputTagSpecDet+"</tag><dbConnection>"+DBConnection+"</dbConnection>"+ChannelSelection,className="LArDetCellParamsComplete")

if 'LArOFCBinFolder' in dir():
    if not 'InputTagSpecDet' in dir():
       InputTagSpecDet = LArCalibFolderTag(LArOFCBinFolder,LArInputTag)

    conddb.addFolder("",LArOFCBinFolder+"<tag>"+InputTagSpecDet+"</tag><dbConnection>"+DBConnection+"</dbConnection><key>LArOFCBinComplete</key>"+ChannelSelection,className="LArOFCBinComplete")

   
##########################################################################
#                                                                        #
#                                 Output                                 #
#                                                                        #
##########################################################################

from LArCalibTools.LArCalibToolsConf import LArParams2Ntuple
LArParams2Ntuple = LArParams2Ntuple( "LArParams2Ntuple" )
LArParams2Ntuple.NtupleName  = "PARAMS"
LArParams2Ntuple.KeyList     =  LArInputKey 
LArParams2Ntuple.AllChannels2Ntuple   =  AllChannels2Ntuple 

topSequence+=LArParams2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
if os.path.exists(OutputRootFileDir+"/"+OutputRootFileName): 
   os.remove(OutputRootFileDir+"/"+OutputRootFileName)  
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputRootFileDir+"/"+OutputRootFileName+"' OPT='NEW'" ]

###########################################################################


###########################################################################
#               Use EventSelector to select IOV                           #
###########################################################################	

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber	= RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent	= 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1


##########################################################################
#          don't remove otherwise infinite loop                          #
##########################################################################

theApp.EvtMax = 1

###########################################################################


svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

###########################################################################

StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel=DEBUG
StoreGateSvc.Dump = True

