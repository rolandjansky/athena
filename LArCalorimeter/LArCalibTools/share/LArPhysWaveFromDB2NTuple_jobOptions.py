import commands
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("LArCalibProcessing/GetInputFiles.py")

#######################################################
#       Run properties
#######################################################

if not 'RunNumber' in dir():
   RunNumber = 88237
   
if not ' GainList' in dir():
    GainList = ["HIGH", "MEDIUM", "LOW"]

if not 'ChannelSelection' in dir():
   ChannelSelection=""

if not 'SaveDerivedInfo' in dir():
   SaveDerivedInfo = True

if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True # Xtalk correction for strips

if not 'Partition' in dir():
      Partition = "EB-EMB"

if not 'LArInputWaveKey' in dir():
   LArInputWaveKey = "LArPhysWave"

if not 'LArInputWaveTag' in dir():
   LArInputWaveTag = ""

#######################################################
#      Delay output name
#######################################################

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = RunNumber

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200"

if not 'DBConnectionWave' in dir():
   DBConnectionWave = DBConnectionCOOL   

## Output

if not 'LArCalibFolderOutput' in dir():
   if StripsXtalkCorr:
     LArCalibFolderOutput = LArCalib_Flags.LArPhysWaveFolderXtlk 
   else:                                                                
     LArCalibFolderOutput = LArCalib_Flags.LArPhysWaveFolder
if not 'LArCalibFolderOutputTag' in dir():
   LArCalibFolderOutputTag = LArCalib_Flags.tagSuffix    
   
if not 'OutputPhysWaveRootFileDir' in dir():
   OutputPhysWaveRootFileDir  = commands.getoutput("pwd")      

if not 'KeyOutput' in dir():  
   KeyOutput = "LArPhysWave" # Key of LArPhysWaveContainer saved in Pool file
   
if not 'BaseFileName' in dir():
   BaseFileName = "LArPhysWave"
   
if StripsXtalkCorr :
   BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","") +"_StripsXtalkCorr"
else :
   BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","")

if not 'OutputPhysWaveRootFileName' in dir():
   OutputPhysWaveRootFileName = BaseFileName+".root"
   
if not 'OutputPhysWavePoolFileName' in dir():
   OutputPhysWavePoolFileName = BaseFileName+".pool.root"

if not 'OutputObjectSpecPhysWave' in dir():
   OutputObjectSpecPhysWave = "LArPhysWaveContainer#"+KeyOutput+"#"+LArCalibFolderOutput
   OutputTagSpecPhysWave = LArCalibFolderTag(LArCalibFolderOutput,LArCalibFolderOutputTag)

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
if not 'LArInputWaveFolder' in dir():
  if StripsXtalkCorr:
    conddb.addFolder("",LArCalib_Flags.LArPhysWaveFolderXtlk+"<key>"+LArInputWaveKey+"</key><dbConnection>"+DBConnectionWave+"</dbConnection>"+ChannelSelection)
  else:
    conddb.addFolder("",LArCalib_Flags.LArPhysWaveFolder+"<key>"+LArInputWaveKey+"</key><dbConnection>"+DBConnectionWave+"</dbConnection>"+ChannelSelection)
else:    
    InputTagSpecPhysWave = LArCalibFolderTag(LArInputWaveFolder,LArInputWaveTag)
    print ('Input tag: ',InputTagSpecPhysWave," in folder: ",LArInputWaveFolder)
    conddb.addFolder("",LArInputWaveFolder+"<key>"+LArInputWaveKey+"</key><dbConnection>"+DBConnectionWave+"</dbConnection>"+ChannelSelection)

   
##########################################################################
#                                                                        #
#                                 Output                                 #
#                                                                        #
##########################################################################

if (WriteNtuple):
   from LArCalibTools.LArCalibToolsConf import LArPhysWaves2Ntuple
   LArPhysWaves2Ntuple = LArPhysWaves2Ntuple( "LArPhysWaves2Ntuple" )
   LArPhysWaves2Ntuple.NtupleName  = "PhysWave"
   LArPhysWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
   LArPhysWaves2Ntuple.KeyList     = [ LArInputWaveKey ]
   
   topSequence+=LArPhysWaves2Ntuple
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName): 
      os.remove(OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName+"' OPT='NEW'" ]

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

