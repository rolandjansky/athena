
import commands
###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to reconstruction a DELAY runs in all 3 gains
#
# Last update: 06/03/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("RecExCommission/GetInputFiles.py")

#######################################################
#       Run properties
#######################################################

if not 'RunNumber' in dir():
   RunNumber = 88237
   
if not 'FilePrefix' in dir():
     FilePrefix = "daq.Delay"

if not 'GainList' in dir():
   GainList = [ "HIGH", "MEDIUM", "LOW" ]

if not 'GroupingType' in dir():
   GroupingType = "ExtendedFeedThrough"

if not 'ChannelSelection' in dir():
   # read all
   ChannelSelection = "<channelSelection>35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,1005"
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>"

if not 'SaveDerivedInfo' in dir():
   SaveDerivedInfo = True

if not 'SaveJitter' in dir():
   SaveJitter = True
  
if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True # Xtalk correction for strips

if not 'Partition' in dir():
      Partition = "EB-EMB"
      
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
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200;user=ATLAS_COOL_READER"

## Output

if not 'LArCalibFolderOutputTag' in dir():
   LArCalibFolderOutputTag = LArCalib_Flags.tagSuffix    
   
if not 'OutputCaliWaveRootFileDir' in dir():
   OutputCaliWaveRootFileDir  = commands.getoutput("pwd")      

if not 'KeyOutput' in dir():  
   KeyOutput = "LArCaliWave" # Key of LArCaliWaveContainer saved in Pool file
   
if not 'BaseFileName' in dir():
   BaseFileName = "LArCaliWave"
   
if StripsXtalkCorr :
   BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","") +"_StripsXtalkCorr"
else :
   BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","")

if not 'OutputCaliWaveRootFileName' in dir():
   OutputCaliWaveRootFileName = BaseFileName+".root"
   
if not 'OutputCaliWavePoolFileName' in dir():
   OutputCaliWavePoolFileName = BaseFileName+".pool.root"

if not 'OutputObjectSpecCaliWave' in dir():
   if StripsXtalkCorr:
      OutputObjectSpecCaliWave = "LArCaliWaveContainer#"+KeyOutput+"#"+LArCalib_Flags.LArCaliWaveFolderXtlk
      OutputTagSpecCaliWave = LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolderXtlk,LArCalibFolderOutputTag)
   else:
      OutputObjectSpecCaliWave = "LArCaliWaveContainer#"+KeyOutput+"#"+LArCalib_Flags.LArCaliWaveFolder
      OutputTagSpecCaliWave = LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolder,LArCalibFolderOutputTag)

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

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

conddb.addFolder("","/LAR/BadChannels/BadChannels<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
conddb.addFolder("","/LAR/BadChannels/MissingFEBs<dbConnection>"+DBConnectionCOOL+"</dbConnection>")

svcMgr.PoolSvc.ReadCatalog += ["prfile:poolcond/PoolCat_oflcond.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000005.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000006.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_diskbuffer_afs.xml",
                               "xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond08_data.000001.lar.COND_castor.xml"]

## The reference is the Oracle DB
if StripsXtalkCorr:
   conddb.addFolder("LAR",LArCalib_Flags.LArCaliWaveFolderXtlk+"<key>LArCaliWave</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)
else:
   conddb.addFolder("LAR",LArCalib_Flags.LArCaliWaveFolder+"<key>LArCaliWave</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)

   
if ( len(PoolFileList)>0 ):
   
   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

##########################################################################
#                                                                        #
#                                 Output                                 #
#                                                                        #
##########################################################################

if (WriteNtuple):
   from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple
   LArCaliWaves2Ntuple = LArCaliWaves2Ntuple( "LArCaliWaves2Ntuple" )
   LArCaliWaves2Ntuple.NtupleName  = "CALIWAVE"
   LArCaliWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
   LArCaliWaves2Ntuple.SaveJitter = SaveJitter
   LArCaliWaves2Ntuple.KeyList     = [ KeyOutput ]
   
   topSequence+=LArCaliWaves2Ntuple
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName): 
      os.remove(OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName+"' OPT='NEW'" ]

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

