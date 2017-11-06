import commands

if not "SuperCells" in dir():
   SuperCells=False
   
if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
if SuperCells:     include("LArCalibProcessing/LArCalib_FlagsSC.py")

#######################################################
#                Run properties
#######################################################

if not 'SubDet' in dir():
   SubDet = "Barrel"
   
if not 'RunNumberList' in dir():
   RunNumberList = [ '0026924' ]

if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"     

if not 'GainList' in dir():
   GainList = [ "HIGH", "MEDIUM", "LOW" ]

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"

if not 'ChannelSelection' in dir():
   # read all
   ChannelSelection = " "
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>"   

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"


#######################################################
#                Monitoring properties
#######################################################

if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = True

if not 'online' in dir():
   online = False
   
#######################################################
#       AutoCorrelation  properties
#######################################################

if 'NColl' not in dir():
   NColl=0

if 'bunchSpace' not in dir():
   bunchSpace = 25

if not 'NSamples' in dir():
   NSamples = 32   
   
if not 'WhichSample' in dir():
   WhichSample = -1

if not 'InputDB' in dir():
   InputDB = LArCalib_Flags.OutputDB

if 'InputSQLiteFile' in dir():
   InputDB = DBConnectionFile(OutputSQLiteFile)   


#######################################################
#      AutoCorrelation  output name
#######################################################

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = int(RunNumberList[0])
   
if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"   

if not 'OutputPedAutoCorrRootFileDir' in dir():
   OutputPedAutoCorrRootFileDir  = commands.getoutput("pwd")
   
if not 'OutputPedAutoCorrPoolFileDir' in dir():
   OutputPedAutoCorrPoolFileDir  = commands.getoutput("pwd")

if not 'ACLArCalibFolderTag' in dir():
   rs=FolderTagResover()
   ACLArCalibFolderTag = rs.getFolderTagSuffix(LArCalib_Flags.LArPhysAutoCorrFolder)
   del rs #Close DB connection
   
   
if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)   

if not 'KeyOutputAC' in dir():  
   KeyOutputAC      = "LArPhysAutoCorr" 

if not 'BaseFileNamePedAutoCorr' in dir():
   BaseFileNameAutoCorr = "LArPhysAutoCorr"
    
for RunNumber in RunNumberList :
   BaseFileNameAutoCorr = BaseFileNameAutoCorr + "_"
   BaseFileNameAutoCorr = BaseFileNameAutoCorr + str(RunNumber)+"_"+Partition.replace("*","") 
   
if not 'OutputAutoCorrRootFileName' in dir():
   OutputAutoCorrRootFileName  = BaseFileNameAutoCorr+ ".root"
    
if not 'OutputAutoCorrPoolFileName' in dir():
   OutputAutoCorrPoolFileName  = BaseFileNameAutoCorr + ".pool.root"

if not 'OutputObjectSpecAutoCorr' in dir():
   OutputObjectSpecAutoCorr = "LArAutoCorrComplete#"+KeyOutputAC+"#"+LArCalib_Flags.LArPhysAutoCorrFolder
   OutputTagSpecAutoCorr=LArCalibFolderTag(LArCalib_Flags.LArPhysAutoCorrFolder,"_mu_"+str(NColl)+ACLArCalibFolderTag)

#######################################################################################
# print summary
#######################################################################################

PedestalAutoCorrLog = logging.getLogger( "PedestalAutoCorrLog" )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " ***           LAr PhysAutoCorr summary               *** " )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " RunNumber                         = "+str(RunNumberList) )
PedestalAutoCorrLog.info( " SubDetector                       = "+SubDet )
PedestalAutoCorrLog.info( " Partition                         = "+Partition )
PedestalAutoCorrLog.info( " Type                              = Pedestal " )
PedestalAutoCorrLog.info( " LArGain                           = "+str(GainList) )
PedestalAutoCorrLog.info( " LArCalibFolderTag              = "+"_mu_"+str(NColl)+ACLArCalibFolderTag )
if WriteNtuple:
   PedestalAutoCorrLog.info( " OutputAutoCorrRootFullFileName = "+OutputAutoCorrRootFileDir + "/" + OutputAutoCorrRootFileName )
PedestalAutoCorrLog.info( " OutputAutoCorrPoolFullFileName    = "+OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName )
PedestalAutoCorrLog.info( " OutputObjectSpecAutoCorr          = "+OutputObjectSpecAutoCorr )
PedestalAutoCorrLog.info( " OutputTagSpecAutoCorr             = "+OutputTagSpecAutoCorr )
PedestalAutoCorrLog.info( " IOVBegin                          = "+str(IOVBegin) )
PedestalAutoCorrLog.info( " IOVEnd                            = "+str(IOVEnd) )
PedestalAutoCorrLog.info( " LArCalibOutputDB                  = "+OutputDB )
PedestalAutoCorrLog.info( " ======================================================== " )

#######################################################################################
#include ("LArConditionsCommon/LArMinimalSetup.py")
include("LArCalibProcessing/LArCalib_MinimalSetup.py")

GlobalTag = 'CONDBR2-BLKPA-2017-01'

#Geometry = 'ATLAS-GEO-21-00-01'
#from AthenaCommon.DetFlags import DetFlags
#DetFlags.all_setOff()
#DetFlags.LAr_setOn()
#DetFlags.Tile_setOn()
#DetFlags.digitize.all_setOff()

#from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetGeo.set_Value_and_Lock('atlas')
#globalflags.DataSource.set_Value_and_Lock('data')
#globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = bunchSpace
jobproperties.Beam.numberOfCollisions = float(NColl)

# dont load Shape from CONDBR2 
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.useShape.set_Value_and_Lock(False)

import AthenaCommon.AtlasUnixGeneratorJob

# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool

#from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.DetDescrVersion=Geometry

#from AtlasGeoModel import SetGeometryVersion
#from AtlasGeoModel import GeoModelInit


## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += [ "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_oflcond.xml" ]

from LArROD.LArRODFlags import larRODFlags
larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True)
larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
#include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

if SuperCells:
  svcMgr.LArFlatConditionSvc.DoSuperCells = True
  svcMgr.LArFlatConditionSvc.DoRegularCells = False
  #svcMgr.LArFlatConditionSvc.PedestalSCOutput = "Pedestal"
  svcMgr.LArFlatConditionSvc.uA2MeVSCInput = "LAruA2MeVFlat"
  svcMgr.LArFlatConditionSvc.DAC2uAVSCInput = "LArDAC2uAFlat"
  #svcMgr.LArFlatConditionSvc.RampSCInput = "LArRamp"
  #svcMgr.LArFlatConditionSvc.PedestalSCInput = "Pedestal"
  

# Temperature folder
#conddb.addFolder("DCS_OFL","/LAR/DCS/FEBTEMP")
#svcMgr.EventSelector.InitialTimeStamp = 1284030331
#import cx_Oracle
#import time
#import datetime
#connection=cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlr")
#cursor=connection.cursor()
#sRequest=("SELECT RUNNR,CREATION_TIME FROM SFO_TZ_RUN WHERE RUNNR='%s'")%(RunNumberList[0])
#cursor.execute(sRequest)
#times= cursor.fetchall()
#d=times[0][1]
#iovtemp=int(time.mktime(d.timetuple()))
##print "Setting timestamp for run ",RunNumberList[0]," to ",iovtemp
##svcMgr.IOVDbSvc.forceTimestamp = 1283145454
#svcMgr.IOVDbSvc.forceTimestamp = iovtemp

#if ( doLArCalibDataQuality  ) :

#   ## The reference is the Oracle DB
#   conddb.addFolder("",LArCalib_Flags.LArAutoCorrFolder+"<key>LArAutoCorrRef</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)

if SuperCells:
   conddb.addFolder("","/LAR/IdentifierOfl/OnOffIdMap_SC<db>COOLOFL_LAR/OFLP200</db><tag>LARIdentifierOflOnOffIdMap_SC-000</tag>") 

#AutoCorrelation from data electronics noise
if not SuperCells:
  conddb.addFolder("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOfl/AutoCorrs/AutoCorr<key>LArAutoCorrRef</key>")
  conddb.addOverride("/LAR/ElecCalibOfl/AutoCorrs/AutoCorr","LARElecCalibOflAutoCorrsAutoCorr-RUN2-UPD3-00")
if SuperCells:
  conddb.addFolder("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/AutoCorrs/AutoCorr<key>LArAutoCorrRef</key>")
  conddb.addOverride("/LAR/ElecCalibOflSC/AutoCorrs/AutoCorr","LARElecCalibOflSCAutoCorrsAutoCorr-UPD3-00")

#load fsampl, MinBias Average and PulseShape 32 samples from OFLP200
from IOVDbSvc.CondDB import conddb
if not SuperCells:
  conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/Shape",forceMC=True)
  conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/MinBias",forceMC=True)
  conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/fSampl",forceMC=True)
  # for OFC old numbers !!!
  conddb.addOverride("/LAR/ElecCalibMC/fSampl","LARElecCalibMCfSampl-G496-19213-FTFP_BERT_BIRK")
  #conddb.addOverride("/LAR/ElecCalibMC/fSampl","LARElecCalibMCfSampl-G4101-20371-FTFP_BERT_BIRK_v2")
  conddb.addOverride("/LAR/ElecCalibMC/Shape","LARElecCalibMCShape-Apr2010")
  conddb.addOverride("/LAR/ElecCalibMC/MinBias","LARElecCalibMCMinBias-mc15-s2081")
  #conddb.addOverride("/LAR/ElecCalibMC/MinBias","LARElecCalibMCMinBias-mc16-ofc25mc15mu20-25ns-A3MinBias_1")
if SuperCells:
  # Will for now use a local db for SC coefficients
  InputDBLocalCalib = "sqlite://;schema=localCalibConstants.db;dbname=CONDBR2"
  
  print "LAr conf flags FEB gain threshold"
  print larCondFlags.useLArFEBGainThresholds()
  
  conddb.addFolder("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/Pedestals/Pedestal")
  conddb.addFolder("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/Ramps/RampLinea")
  conddb.addFolder("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/MphysOverMcal/RTM")
  conddb.addOverride("/LAR/ElecCalibOflSC/Pedestals/Pedestal","LARElecCalibOflSCPedestalsPedestal-UPD3-00")
  conddb.addOverride("/LAR/ElecCalibOflSC/Ramps/RampLinea","LARElecCalibOflSCRampsRampLinea-UPD3-00")
  conddb.addOverride("/LAR/ElecCalibOflSC/MphysOverMcal/RTM","LARElecCalibOflSCMphysOverMcalRTM-UPD3-00")
  
  conddb.addFolder("","<dbConnection>"+InputDBLocalCalib+"</dbConnection>/LAR/ElecCalibFlatSC/DAC2uA")
  conddb.addFolder("","<dbConnection>"+InputDBLocalCalib+"</dbConnection>/LAR/ElecCalibFlatSC/uA2MeV")
  conddb.addOverride("/LAR/ElecCalibFlatSC/DAC2uA","LARElecCalibFlatSCDAC2uA-000")
  conddb.addOverride("/LAR/ElecCalibFlatSC/uA2MeV","LARElecCalibFlatSCuA2MeV-000")
  
  conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/Shape",forceMC=True)
  conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/MinBias",forceMC=True)
  conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/fSampl",forceMC=True)
  conddb.addOverride("/LAR/ElecCalibMCSC/Shape","LARElecCalibMCSCShape-000")
  conddb.addOverride("/LAR/ElecCalibMCSC/MinBias","LARElecCalibMCSCMinBias-000")
  conddb.addOverride("/LAR/ElecCalibMCSC/fSampl","LARElecCalibMCSCfSampl-000")


svcMgr.IOVDbSvc.GlobalTag = GlobalTag

if not SuperCells:
  from LArRecUtils.LArAutoCorrTotalToolDefault import LArAutoCorrTotalToolDefault
  theLArAutoCorrTool = LArAutoCorrTotalToolDefault()
  theLArAutoCorrTool.NSamples = NSamples
  theLArAutoCorrTool.keyAutoCorr = "LArAutoCorrRef"
  ToolSvc += theLArAutoCorrTool
if SuperCells:
  from LArRecUtils.LArAutoCorrTotalSCToolDefault import LArAutoCorrTotalSCToolDefault
  theLArAutoCorrTool = LArAutoCorrTotalSCToolDefault()
  theLArAutoCorrTool.NSamples = NSamples
  theLArAutoCorrTool.keyAutoCorr = "LArAutoCorrRef"
  theLArAutoCorrTool.keyShape = "LArShapeSC"
  theLArAutoCorrTool.keyPedestal = "Pedestal"
  theLArAutoCorrTool.keyfSampl = "LARfSamplSC"
  theLArAutoCorrTool.keyMinBias = "LArMinBiasSC"
  ToolSvc += theLArAutoCorrTool


from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrToolToDB
theLArAutoCorrToolToDB = LArAutoCorrToolToDB("LArAutoCorrToolToDB")
theLArAutoCorrToolToDB.AutoCorrTool = theLArAutoCorrTool
theLArAutoCorrToolToDB.GroupingType = "ExtendedSubDetector"
theLArAutoCorrToolToDB.AutoCorrKey = KeyOutputAC
topSequence += theLArAutoCorrToolToDB

# 
#ToolSvc.LArADC2MeVSCToolDefault.UseFEBGainTresholds=False
if not SuperCells:
  ToolSvc.LArADC2MeVToolDefault.UseFEBGainTresholds=False
if SuperCells:
  ToolSvc.LArADC2MeVSCToolDefault.UseFEBGainTresholds=False
  ToolSvc.LArADC2MeVSCToolDefault.keyADC2DAC='LArRamp'
  ToolSvc.LArADC2MeVSCToolDefault.keyDAC2uA='LArDAC2uASC'
  ToolSvc.LArADC2MeVSCToolDefault.keyuA2MeV='LAruA2MeVSC'
#
######################################################################
#                                                                    #
#                          Output                                    #
#                                                                    #
######################################################################



if ( WriteNtuple ) :
   
   from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
   LArAutoCorr2Ntuple = LArAutoCorr2Ntuple( "LArAutoCorr2Ntuple" )
   #LArAutoCorr2Ntuple.Nsamples     = NSamples
   LArAutoCorr2Ntuple.Nsamples     = 32
   LArAutoCorr2Ntuple.ContainerKey = KeyOutputAC
   LArAutoCorr2Ntuple.isSC = SuperCells
   
   topSequence += LArAutoCorr2Ntuple

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputAutoCorrRootFileDir + "/" + OutputAutoCorrRootFileName): 
      os.remove(OutputAutoCorrRootFileDir + "/" + OutputAutoCorrRootFileName)
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputAutoCorrRootFileDir + "/" + OutputAutoCorrRootFileName+"' OPT='NEW'" ]


if ( WritePoolFile ) :

        from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
        
        if os.path.exists(OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName): 
           os.remove(OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName)
        OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName,
                                                [OutputObjectSpecAutoCorr],[OutputTagSpecAutoCorr],WriteIOV)
        OutputConditionsAlg.Run1 = IOVBegin
        if IOVEnd>0:
              OutputConditionsAlg.Run2=IOVEnd
        svcMgr.IOVDbSvc.dbConnection  = OutputDB
        
        from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
        svcMgr += IOVRegistrationSvc()
        svcMgr.IOVRegistrationSvc.OutputLevel = INFO
        svcMgr.IOVRegistrationSvc.RecreateFolders = False
       
        

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = int(RunNumberList[0])
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1


###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

###########################################################################
#svcMgr.IOVDbSvc.OutputLevel = DEBUG 

