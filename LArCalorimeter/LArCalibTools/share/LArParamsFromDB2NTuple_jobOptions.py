import commands
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("RecExCommission/GetInputFiles.py")

#######################################################
#       Run properties
#######################################################

if not 'RunNumber' in dir():
   RunNumber = 88237
   
if not ' GainList' in dir():
    GainList = ["HIGH", "MEDIUM", "LOW"]

if not 'ChannelSelection' in dir():
   ChannelSelection=""

if not 'Partition' in dir():
      Partition = "EB-EMB"

if not 'LArCaliInputKey' in dir():
   LArCaliInputKey = ""

if not 'LArDetInputKey' in dir():
   LArDetInputKey = ""

if not 'LArParamsTag' in dir():
   LArParamsTag = ""

if not 'LArInputKey' in dir():
   LArInputKey = [""]

if not 'AllChannels2Ntuple' in dir():
   AllChannels2Ntuple = False

#######################################################
#      Delay output name
#######################################################

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200"

if not 'DBConnection' in dir():
   DBConnection = DBConnectionCOOL   

## Output

if not 'OutputRootFileDir' in dir():
   OutputRootFileDir  = commands.getoutput("pwd")      

if not 'KeyOutput' in dir():  
   KeyOutput = "" # Key of LArPhysWaveContainer saved in Pool file
   
if not 'BaseFileName' in dir():
   BaseFileName = "LArParams"
   
BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","")

if not 'OutputRootFileName' in dir():
   OutputRootFileName = BaseFileName+".root"
   
###########################################################################
#
# Global settings
#
###########################################################################

include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")


## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
svcMgr.IOVDbSvc.DBInstance=""

from IOVDbSvc.CondDB import conddb

conddb.addFolder("","/LAR/BadChannels/BadChannels<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
conddb.addFolder("","/LAR/BadChannels/MissingFEBs<dbConnection>"+DBConnectionCOOL+"</dbConnection>")

svcMgr.PoolSvc.ReadCatalog += ["prfile:poolcond/PoolCat_oflcond.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000005.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000006.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_diskbuffer_afs.xml",
                               "xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond08_data.000001.lar.COND_castor.xml"]

## The reference is the Oracle DB
if  'LArCaliParamsFolder' in dir():
    if not 'InputTagSpecCali' in dir():
       InputTagSpecCali = LArCalibFolderTag(LArCaliParamsFolder,LArInputTag)

#    print 'Input tag: ',InputTagSpecCali," in folder: ",LArCaliParamsFolder
    conddb.addFolder("",LArCaliParamsFolder+"<tag>"+InputTagSpecCali+"</tag><key>"+LArCaliInputKey+"</key><dbConnection>"+DBConnection+"</dbConnection>"+ChannelSelection)

   
if 'LArDetParamsFolder' in dir():
    if not 'InputTagSpecDet' in dir():
       InputTagSpecDet = LArCalibFolderTag(LArDetParamsFolder,LArInputTag)

#    print 'Input tag: ',InputTagSpecDet," in folder: ",LArDetParamsFolder
    conddb.addFolder("",LArDetParamsFolder+"<tag>"+InputTagSpecDet+"</tag><key>"+LArDetInputKey+"</key><dbConnection>"+DBConnection+"</dbConnection>"+ChannelSelection)

   
##########################################################################
#                                                                        #
#                                 Output                                 #
#                                                                        #
##########################################################################

if (WriteNtuple):
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

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################

