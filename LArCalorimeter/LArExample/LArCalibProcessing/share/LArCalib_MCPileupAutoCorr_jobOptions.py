# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from future import standard_library
standard_library.install_aliases()
import subprocess

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

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"   


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
   #InputDB = LArCalib_Flags.OutputDB
   InputDB = DBConnectionCOOL

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

if not 'OutputPedAutoCorrRootFileDir' in dir():
   OutputPedAutoCorrRootFileDir  = subprocess.getoutput("pwd")
   
if not 'OutputPedAutoCorrPoolFileDir' in dir():
   OutputPedAutoCorrPoolFileDir  = subprocess.getoutput("pwd")

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

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = bunchSpace
jobproperties.Beam.numberOfCollisions = float(NColl)

# dont load Shape from CONDBR2 
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.useShape.set_Value_and_Lock(False)

if SuperCells:
   larCondFlags.hasHVCorr.set_Value_and_Lock(False) # here  needed since we don't have HVCorr info
   larCondFlags.hasMphys.set_Value_and_Lock(False) # here  needed since we don't have MPhys info

import AthenaCommon.AtlasUnixGeneratorJob

# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool


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
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

if SuperCells:
   #SC conditiona folders are not set-up yet in LArConditionsCommon_comm_jobOptions.py, should go there at some point
   conddb.addFolderWithTag("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/Pedestals/Pedestal<key>LArPedestal</key>",
                          "LARElecCalibOflSCPedestalsPedestal-UPD3-01",
                          className="LArPedestalComplete")
   conddb.addFolderWithTag("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/Ramps/RampLinea", "LARElecCalibOflSCRampsRampLinea-UPD3-01", className="LArRampComplete")
   conddb.addFolderWithTag("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/MphysOverMcal/RTM", "LARElecCalibOflSCMphysOverMcalRTM-UPD3-00", className="LArMphysOverMcalComplete")
  
   
# for calib data we need:
conddb.addOverride("/CALO/Identifier/CaloTTOnAttrIdMapAtlas","CALOIdentifierCaloTTOnAttrIdMapAtlas-RUN2-0001")
conddb.addOverride("/CALO/Identifier/CaloTTOnOffIdMapAtlas","CALOIdentifierCaloTTOnOffIdMapAtlas-RUN2-0002")
conddb.addOverride("/CALO/Identifier/CaloTTPpmRxIdMapAtlas","CALOIdentifierCaloTTPpmRxIdMapAtlas-RUN2-0000")

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
##print ("Setting timestamp for run ",RunNumberList[0]," to ",iovtemp)
##svcMgr.IOVDbSvc.forceTimestamp = 1283145454
#svcMgr.IOVDbSvc.forceTimestamp = iovtemp


# Idmapping
if SuperCells:
   from LArCabling.LArCablingAccess import LArOnOffIdMappingSC
   LArOnOffIdMappingSC()
else:
   from LArCabling.LArCablingAccess import LArOnOffIdMapping
   LArOnOffIdMapping()


#AutoCorrelation from data electronics noise
if not SuperCells:
   conddb.addFolder("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOfl/AutoCorrs/AutoCorr<key>LArAutoCorrRef</key>",className='LArAutoCorrComplete')
   conddb.addOverride("/LAR/ElecCalibOfl/AutoCorrs/AutoCorr","LARElecCalibOflAutoCorrsAutoCorr-RUN2-UPD3-00")
else:
   conddb.addFolder("","<dbConnection>"+InputDB+"</dbConnection>/LAR/ElecCalibOflSC/AutoCorrs/AutoCorr<key>LArAutoCorrRef</key>",className='LArAutoCorrComplete')
   conddb.addOverride("/LAR/ElecCalibOflSC/AutoCorrs/AutoCorr","LARElecCalibOflSCAutoCorrsAutoCorr-UPD3-01")

from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalCondAlg 
theAutoCorrTotalCondAlg=LArAutoCorrTotalCondAlg()
theAutoCorrTotalCondAlg.Nsamples = NSamples
theAutoCorrTotalCondAlg.isMC = False
theAutoCorrTotalCondAlg.LArAutoCorrObjKey = "LArAutoCorrRef"
theAutoCorrTotalCondAlg.LArAutoCorrTotalObjKey = KeyOutputAC

#load fsampl, MinBias Average and PulseShape 32 samples from OFLP200
from IOVDbSvc.CondDB import conddb
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not SuperCells:
  from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArShape32MC_LArShape32Sym_ as LArShapeSymAlg
  from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasMC_LArMinBiasSym_ as LArMinBiasSymAlg
  from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArfSamplMC_LArfSamplSym_ as LArfSamplSymAlg 
  mcfolders=[("LArShape32MC","/LAR/ElecCalibMC/Shape","LArShape","LARElecCalibMCShape-Apr2010",LArShapeSymAlg),
             ("LArMinBiasMC","/LAR/ElecCalibMC/MinBias","LArMinBias","LARElecCalibMCMinBias-mc16-ofc25mc15mu20-25ns-A3MinBias_1",LArMinBiasSymAlg),
             ("LArfSamplMC","/LAR/ElecCalibMC/fSampl","LArfSampl","LARElecCalibMCfSampl-G4101-20371-FTFP_BERT_BIRK_v2",LArfSamplSymAlg)]
  for className,fldr,key,tag,calg in mcfolders:           
     keynew=key+"Sym"
     conddb.addFolder("LAR_OFL",fldr, forceMC=True, className=className)
     conddb.addOverride(fldr,tag)
     condSeq+=calg(ReadKey=key,WriteKey=keynew)
     
else:
  from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeSC_ as LArShapeCondAlg
  from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArRampSC_ as LArRampCondAlg
  from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMinBiasSC_ as LArMinBiasCondAlg
  from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArfSamplSC_ as LArfSamplCondAlg
  from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArfSamplSC_ as LArfSamplCondAlg
  from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LAruA2MeVSC_ as LAruA2MeVCondAlg
  from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArDAC2uASC_ as LArDAC2uACondAlg


  mcfolders=[("CondAttrListCollection","/LAR/ElecCalibMCSC/Shape","LArShapeSC","LARElecCalibMCSCShape-000",LArShapeCondAlg),
             ("CondAttrListCollection","/LAR/ElecCalibMCSC/Ramp","LArRampSC","LARElecCalibMCSRamp-mV-LSB",LArRampCondAlg),
             ("CondAttrListCollection","/LAR/ElecCalibMCSC/MinBias","LArMinBiasSC","LARElecCalibMCSCMinBias-000",LArMinBiasCondAlg),
             ("CondAttrListCollection","/LAR/ElecCalibMCSC/fSampl","LArfSamplSC","LARElecCalibMCSCfSampl-000",LArfSamplCondAlg),
             ("CondAttrListCollection","/LAR/ElecCalibMCSC/uA2MeV","LAruA2MeVSC","LARElecCalibMCSCuA2MeV-000",LAruA2MeVCondAlg),
             ("CondAttrListCollection","/LAR/ElecCalibMCSC/DAC2uA","LArDAC2uASC","LARElecCalibMCSCDAC2uA-000",LArDAC2uACondAlg),
  ]
  theAutoCorrTotalCondAlg.LArShapeObjKey = "LArShapeSC"
  theAutoCorrTotalCondAlg.LArMinBiasObjKey = "LArMinBiasSC"
  theAutoCorrTotalCondAlg.LArfSamplObjKey = "LArfSamplSC"
  theAutoCorrTotalCondAlg.isSuperCell = True

  for className,fldr,key,tag,calg in mcfolders:           
     conddb.addFolder("LAR_OFL",fldr, forceMC=True, className=className)
     conddb.addOverride(fldr,tag)
     condSeq+=calg(ReadKey=fldr,WriteKey=key)

  from LArRecUtils.LArADC2MeVSCCondAlgDefault import LArADC2MeVSCCondAlgDefault
  LArADC2MeVSCCondAlgDefault(isMC=True)

condSeq += theAutoCorrTotalCondAlg

# need to run Alg to create LArAutoCorrComplete for DB
from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrAlgToDB
theLArAutoCorrAlgToDB=LArAutoCorrAlgToDB()
theLArAutoCorrAlgToDB.LArAutoCorrTotal=KeyOutputAC
theLArAutoCorrAlgToDB.isSC = SuperCells
theLArAutoCorrAlgToDB.GroupingType = "ExtendedSubDetector"
theLArAutoCorrAlgToDB.OutAutoCorrKey = KeyOutputAC
topSequence += theLArAutoCorrAlgToDB

######################################################################
#                                                                    #
#                          Output                                    #
#                                                                    #
######################################################################



if ( WriteNtuple ) :
   
   from LArCabling.LArCablingAccess import LArCalibIdMappingSC
   LArCalibIdMappingSC()

   from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
   LArAutoCorr2Ntuple = LArAutoCorr2Ntuple( "LArAutoCorr2Ntuple" )
   LArAutoCorr2Ntuple.Nsamples     = NSamples
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

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber         = int(RunNumberList[0])
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
##########################################################################
#          don't remove otherwise infinite loop                          #
##########################################################################

theApp.EvtMax                          = 1


###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

###########################################################################
