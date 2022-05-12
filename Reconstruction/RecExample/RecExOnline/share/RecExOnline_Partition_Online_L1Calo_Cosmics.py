# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

## Job options for Running the L1Calo Athena Online Monitoring
## A few notes:
## At the moment one needs to edit the RecExOnline_monitoring.py 
##    and define the doLVL1CaloMon variable and replace doTrigMon
##    in the statement:
##       DQMonFlags.useTrigger(doTrigMon)
##       DQMonFlags.doLVL1CaloMon(doTrigMon)
##    with doLVL1CaloMon. Then add doLVL1CaloMon to
##    the 'orMon' list.
## I think that is all you need. Cheers, Taylor


# used by RecExOnline to setup Emon Service
useEmon           = True


##################
## -- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)

import os

# set partition name (default is ATLAS)  
partitionName = os.getenv("TDAQ_PARTITION","ATLAS")

# Debug for ATLASDQ-853: Crash with traceback instead of printing errors and continuing (May 2021)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError = False # This will force Athena to crash in case of errors (for debugging)
athenaCommonFlags.BSRDOInput.set_Value_and_Lock([]) # Fix from ATLASDQ-853 (June 2021)

# New config flags (Feb. 2022)  
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# Fix for ID monitoring errors (ATLASDQ-853; Sept. 2021)
ID = ConfigFlags.DQ.Steering.InDet
ID.doGlobalMon=False
ID.doAlignMon=False

# Enable L1Topo monitoring (Oct. 2021 pilot beams)
ConfigFlags.DQ.Steering.doLVL1InterfacesMon=True

## ------------------------------------------- set online defaults for new config flags
# Fix for autoconfig problem ATR-22872 (April 2021)
from AthenaConfiguration.AutoConfigOnlineRecoFlags import autoConfigOnlineRecoFlags
ConfigFlags.Trigger.triggerConfig = 'DB' # temporary 02/2021
autoConfigOnlineRecoFlags(ConfigFlags, partitionName)

# Fix for unavailable LBLB info (ATR-23464)
ConfigFlags.DQ.enableLumiAccess = False
ConfigFlags.DQ.Environment = "online"

# Fix for missing output file ATR-22872 (May 2021)
from AthenaMonitoring.DQMonFlags import DQMonFlags
# Added flags to run AthenaMT
DQMonFlags.doNewMonitoring.set_Value_and_Lock(True) # Change to true to enable MT (needs --threads = 1 to run)
DQMonFlags.doHLTMon.set_Value_and_Lock(False)
DQMonFlags.monManFileKey.set_Value_and_Lock('') # Set top-level path 
DQMonFlags.histogramFile.set_Value_and_Lock('monitoring.root')

# Don't use the trigger decision tool (ATR-24589, Nov. 2021)
DQMonFlags.useTrigger.set_Value_and_Lock(False)
ConfigFlags.DQ.triggerDataAvailable = True # KW test Nov. 22, doesn't work

# To get rid of rogue DQT, PFO, and TopoCluster monitoring plots
DQMonFlags.doJetInputsMon=False
DQMonFlags.doDataFlowMon=False

# set name of this publisher as it will appear in IS (default is "l1calo-athenaHLT"; change to something sensible for testing)
publishName       = "l1calo-athenaHLT-cosmics"

# name of the stream type (physics,express, etc.)
streamType = os.getenv("L1CALO_PTIO_STREAM_TYPE","physics")

# name of the stream (Egamma,JetTauEtmiss,MinBias,Standby, etc.)
# this can be a colon(:) separated list of streams that use
# the 'streamLogic' to combine
# stream for 2016 HI run
streamName = os.getenv("L1CALO_PTIO_STREAM_NAME","CosmicCalo:Standby")

# logic used to combine multiple streams
# can be set to 'Ignore' which means the stream selection isn't used
streamLogic = os.getenv("L1CALO_PTIO_STREAM_LOGIC","Or")
if(partitionName == "L1CaloStandalone"):
   streamLogic = "Ignore"

# name of L1 items to select
# can be colon(:) separated list of L1 items
lvl1Name          = ''

# logic used to combined multiple L1 items
# can be set to 'Ignore' which means the L1 selection isn't used
lvl1Logic         = 'Ignore'

# set the Sampler Key Type name (default is SFI)
if ( not os.environ.get("L1CALO_PTIO_KEY") ):
  if( partitionName == "L1CaloStandalone" ):
    keyname = 'REB'
  else:
    keyname = 'dcm' #'SFI'
else:
  keyname = os.environ.get("L1CALO_PTIO_KEY")

# set the Sampler count (default is 15)
keycount = int(os.environ.get("L1CALO_PTIO_KEY_COUNT","25"))

# event buffer size for each sampler
buffersize        = 10

# time in seconds between updating plots
updateperiod      = 30

# timeout (not sure what this does)
timeout           = 240000

# IS server onwhich to create this provider
isserverName    = 'Histogramming'

# this is not the global monitoring
isGlobalMonitoring = False


#from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag("CONDBR2-HLTP-2014-00")
#conddb.setGlobalTag("CONDBR2-ES1PA-2014-00") #crash


############
## -- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)

#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
#usePickleConfig   = False
#pickleconfigfile  = './ami_recotrf.pickle'
#DataSource        = 'data'
#InputFormat       = 'bytestream'
#fileName         = './0.data'
#doESD             = True
#writeESD          = False
#doAOD             = False
#writeAOD          = False
isOnlineStateless = True
#beamType = 'collisions' #'collisions' # default: 'cosmics'
#is_T0_project_tag = 'cos010'
#is_T0_project_tag = "data_test"
#is_T0_project_tag = "data14_cos"
#is_T0_project_tag = "data15_13TeV"

#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
#ConditionsTag     = 'COMCOND-HLTP-004-01'
#DetDescrVersion   = 'ATLAS-GEO-20-00-01'

from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-ES1PA-2014-00")
#globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-HLTP-2015-05")
#--->>>######globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-HLTP-2015-08")
#globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-HLTP-2016-01")
#globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")
##ConditionsTag = 'CONDBR2-ES1PA-2014-00'
##DetDescrVersion = 'ATLAS-R2-2015-01-01-00' 
#DetDescrVersion = 'ATLAS-R2-2015-03-01-00' 
#--->>>######DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
#DetDescrVersion = 'ATLAS-R2-2016-00-01-00'

#doPixelOnlyMon   = False

# unlimit max number of events
evtMax            = -1

###########
## -- flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = False
# following set to default values from JOs
#doInDet     = doAllReco
#doMuon      = doAllReco
#doLArg      = doAllReco
#doTile      = doAllReco
#doLucid     = False
#doHist      = True
#doJiveXML   = False
#doEgammaTau = doAllReco

###########
## -- flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
#doAllReco   = True
# following set to default values from JOs
#doInDet     = False
#doMuon      = True
#doLArg      = doAllReco
#doTile      = doAllReco
#doLucid     = False
#doHist      = False
#doJiveXML   = False
#doEgammaTau = False
#doLArg      = False
#doTile      = False


#doCommissioning = False

doTrigger = False  

################
## -- flags set in: RecExOnline_monitoring.py (from RecExOnline_jobOptions.py)

# don't need all the monitoring enabled just for L1Calo
doAllMon  = False
#doCaloMon = doAllMon
#doPhysMon = doAllMon
#doIDMon   = doAllMon
#doPixelOnlyMon = False
#doSCTMon  = doAllMon
#doMuonMon = True
#doTRTMon  = doAllMon
#doTrigMon = doAllMon
doLVL1CaloMon = True
#doHLTMon  = doTrigMon
#doCTPMon  = doAllMon
#doCTPMon = True
#doLucidMon= doAllMon
isOnline  = True

rec.doInDet.set_Value_and_Lock(False)
from JetRec.JetRecFlags import jetFlags
jetFlags.useTracks.set_Value_and_Lock(False)
jetFlags.usePFlow.set_Value_and_Lock(False)
jetFlags.useMuonSegments.set_Value_and_Lock(False)


#rec.doMuon.set_Value_and_Lock(False)

# KW fix for CaloExtensionBuilder conflict (ATLASDQ-853)
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(False)
recAlgs.doTrackParticleCellAssociation.set_Value_and_Lock(False)

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

# Don't forget to lock the config flags
ConfigFlags.lock() #KW test Feb. 22 
ConfigFlags.dump()

# Fix for beamspot conditions bug, Feb. 2022 (ATR-25008)
from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
CAtoGlobalWrapper(BeamSpotCondAlgCfg, ConfigFlags)


printfunc (' ')
printfunc ('-------------------------------------------------------------')
printfunc ('|     RecExOnline_globalconfig settings')
printfunc ('| usePickleConfig    = ',usePickleConfig)
printfunc ('| DataSource         = ',DataSource)
printfunc ('| InputFormat        = ',InputFormat)
printfunc ('| fileName           = ',fileName)
#printfunc ('| doESD              = ',doESD)
printfunc ('| writeESD           = ',writeESD)
#printfunc ('| doAOD              = ',doAOD)
printfunc ('| writeAOD           = ',writeAOD)
printfunc ('| isOnlineStateless  = ',isOnlineStateless)
printfunc ('| beamType           = ',beamType)
printfunc ('| is_T0_project_tag  = ',is_T0_project_tag)
printfunc ('| ConditionsTag      = ',ConditionsTag)
#printfunc ('| DetDescrVersion    = ',DetDescrVersion)
printfunc ('| doPixelOnlyMon     = ',doPixelOnlyMon)
printfunc ('| evtMax             = ',evtMax)
printfunc ('-------------------------------------------------------------')
printfunc ('|     RecExOnline_recoflags settings')
printfunc ('| doAllReco          = ',doAllReco)
printfunc ('| doInDet            = ',doInDet)
printfunc ('| doMuon             = ',doMuon)
printfunc ('| doLArg             = ',doLArg)
printfunc ('| doTile             = ',doTile)
printfunc ('| doLucid            = ',doLucid)
#printfunc ('| doTrigger          = ',doTrigger)
#printfunc ('| doHist             = ',doHist)
#printfunc ('| doJiveXML          = ',doJiveXML)
printfunc ('| doEgammaTau        = ',doEgammaTau)
printfunc ('| doCommissioning    = ',doCommissioning)
printfunc ('-------------------------------------------------------------')
printfunc ('|     RecExOnline_monitoring settings')
printfunc ('| doAllMon           = ',doAllMon)
printfunc ('| doCaloMon          = ',doCaloMon)
printfunc ('| doPhysMon          = ',doPhysMon)
printfunc ('| doHLTMon           = ',doHLTMon)
printfunc ('| doLVL1CaloMon      = ',doLVL1CaloMon)
printfunc ('| doCTPMon           = ',doCTPMon)
printfunc ('| doIDMon            = ',doIDMon)
printfunc ('| doPixelOnlyMon     = ',doPixelOnlyMon)
printfunc ('| doSCTMon           = ',doSCTMon)
printfunc ('| doMuonMon          = ',doMuonMon)
printfunc ('| doTRTMon           = ',doTRTMon)
printfunc ('| doLucidMon         = ',doLucidMon)
printfunc ('| isOnline           = ',isOnline)
printfunc ('-------------------------------------------------------------')
printfunc ('|     RecExOnline: emonsvc settings')
printfunc ('| partitionName      = ',partitionName)
printfunc ('| publishName        = ',publishName)
printfunc ('| streamNames        = ',streamName)
printfunc ('| streamType         = ',streamType)
printfunc ('| streamLogic        = ',streamLogic)
printfunc ('| lvl1Name           = ',lvl1Name)
printfunc ('| lvl1Logic          = ',lvl1Logic)
printfunc ('| keyname            = ',keyname)
printfunc ('| keycount           = ',keycount)
printfunc ('| buffersize         = ',buffersize)
printfunc ('| updateperiod       = ',updateperiod)
printfunc ('| timeout            = ',timeout)
printfunc ('| isserverName       = ',isserverName)
printfunc ('| isGlobalMonitoring = ',isGlobalMonitoring)
printfunc ('--------------------------------------------------------------')

#from AthenaCommon.AppMgr import ServiceMgr
#from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
#ServiceMgr += L1CaloCondSvc()
#from IOVDbSvc.CondDB import conddb
#L1CaloFolderList = []
#L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"]
#L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/PprChanCalib"]
#L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"]
#L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults"]
#for l1calofolder in L1CaloFolderList:
#  conddb.addFolderWithTag("TRIGGER", l1calofolder, "HEAD")

#L1CaloDb=""
#if not 'L1CaloDbConnection' in dir():
#    if 'L1CaloSqliteDB' in dir():
#        L1CaloDbConnection="<dbConnection>sqlite://;schema=" + L1CaloSqliteDB + ";dbname=L1CALO</dbConnection>"
#    else:
#        L1CaloDb="TRIGGER"
#        L1CaloDbConnection=""

##conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib")
##conddb.blockFolder("/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib")
#conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels")
#conddb.blockFolder("/TRIGGER/Receivers/Factors/CalibGains")
##conddb.blockFolder("/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults")
#conddb.blockFolder("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers")



#L1CaloDbTag = "<tag>HEAD</tag>"
#
#L1CaloFolderList = []
#L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"]
#L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"]
#L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]

#for l1calofolder in L1CaloFolderList:
#    if not conddb.folderRequested(l1calofolder):
#        conddb.addFolder(L1CaloDb, L1CaloDbConnection + l1calofolder + L1CaloDbTag)


#from IOVDbSvc.CondDB import conddb
##conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib")
##conddb.blockFolder("/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib")
##conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels")
##conddb.blockFolder("/TRIGGER/Receivers/Factors/CalibGains")
##conddb.blockFolder("/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults")
##conddb.blockFolder("/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults")
##conddb.blockFolder("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib<tag>HEAD</tag>")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels<tag>HEAD</tag>")
##conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/Receivers/Factors/CalibGains<tag>HEAD</tag>")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults<tag>HEAD</tag>")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Conditions/DisabledTowers<tag>HEAD</tag>")
#
#from IOVDbSvc.CondDB import conddb
#conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib")
#conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels")
#conddb.blockFolder("/TRIGGER/Receivers/Factors/CalibGains")
#conddb.blockFolder("/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults")
#conddb.blockFolder("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib<tag>HEAD</tag>")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels<tag>HEAD</tag>")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/Receivers/Factors/CalibGains<tag>HEAD</tag>")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults<tag>HEAD</tag>")
#conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Conditions/DisabledTowers<tag>HEAD</tag>")
#
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-02')

#--->>>######conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-03')

#from IOVDbSvc.CondDB import conddb
#conddb.addFolder("TRIGGER","/TRIGGER/HLT/Menu <tag>HEAD</tag>")
#conddb.addFolder("TRIGGER","/TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>")
#conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>")
#conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Menu <tag>HEAD</tag>")
#conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Prescales <tag>HEAD</tag>")



#theApp.CreateSvc  += ["StoreGateSvc/StoreGateSvc" ]
#ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
#ByteStreamAddressProviderSvc.TypeNames += ["/Run/L1TopoToCTPLocation"]
#ProxyProviderSvc = Service( "ProxyProviderSvc" )
#ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
#svcMgr.ProxyProviderSvc.ProviderNames += [ "/Run/L1TopoToCTPLocation" ]


# added for testing purposes
# need to for the run number in the test data partitions
# fix for missing folder /TRIGGER/LUMI/LBLESTONL
#if (partitionName.find("L1CaloStandalone") >= 0) or (partitionName.find("Test_dataProvider") >= 0) :

if (partitionName.find("L1CaloStandalone") >= 0) or (partitionName.find("ATLAS") >= 0) :
#if (partitionName.find("L1CaloStandalone") >= 0) :
  printfunc ("L1Calo Monitoring is overriding the run number and lumiblock number.")
  import ispy   ## this should retrieve the run number automatically as done in RecExOnline_globalconfig.py
  from ispy import *
  from ipc import IPCPartition
  from ispy import ISObject
  p2  = ispy.IPCPartition(partitionName)
  obj = ispy.ISObject(p2, 'RunParams.RunParams', 'RunParams')
  obj.checkout()
  is_run_number = obj.run_number
  svcMgr.IOVDbSvc.forceRunNumber = is_run_number
  #svcMgr.IOVDbSvc.forceRunNumber=313285 #313063 #312649(HI) #312424(HI) #309640 #271733 #182519 #238735
  svcMgr.IOVDbSvc.forceLumiblockNumber=1
  printfunc ("L1Calo Monitoring set run to ",svcMgr.IOVDbSvc.forceRunNumber,"and lumi block to",svcMgr.IOVDbSvc.forceLumiblockNumber)

  #from IOVDbSvc.CondDB import conddb
  #conddb.addFolder("TRIGGER","/TRIGGER/HLT/Menu <tag>HEAD</tag>") 
  #conddb.addFolder("TRIGGER","/TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>")
  #conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>")
  #conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Menu <tag>HEAD</tag>")
  #conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Prescales <tag>HEAD</tag>")


  from IOVDbSvc.CondDB import conddb
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/HLT/Menu<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/HLT/HltConfigKeys<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/HLT/Prescales<tag>HEAD</tag>")

  conddb.addFolder("", "<db>oracle://ATONR_COOL;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2</dbConnection>/TRIGGER/HLT/PrescaleKey<tag>HEAD</tag>")
  conddb.addFolder("", "<db>oracle://ATONR_COOL;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2</dbConnection>/TRIGGER/HLT/Groups<tag>HEAD</tag>")

  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/Lvl1ConfigKey<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/Menu<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/ItemDef<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/BunchGroupKey<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/BunchGroupDescription<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/BunchGroupContent<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/Prescales<tag>HEAD</tag>")
  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/Thresholds<tag>HEAD</tag>")

  conddb.addFolder("", "<db>COOLONL_TRIGGER/CONDBR2</db>/TRIGGER/LVL1/CTPCoreInputMapping<tag>HEAD</tag>")



  # only offline
  from RecExConfig.InputFilePeeker import inputFileSummary
  printfunc ("inputFileSummary =",inputFileSummary)
  if inputFileSummary.__contains__('bs_metadata'):
    # get the run number and lumi_block for the input
    run_number = inputFileSummary['bs_metadata']['run_number']

    pointintime = (int(run_number)<<32) + int(lumi_block) # start from lumiblock 0 (or 1?)

    DBInstance = svcMgr.IOVDbSvc.properties()['DBInstance']
    printfunc ("L1Calo Monitoring check DBInstance ",DBInstance)
    connstring = "COOLONL_TRIGGER/"+str(DBInstance)
    from CoolConvUtilities.AtlCoolLib import indirectOpen
    coolDB=indirectOpen(connstring,oracle='True')
    SMKfolder=coolDB.getFolder('/TRIGGER/HLT/HltConfigKeys')

    retrieved_obj=SMKfolder.findObject(pointintime,0)
    retrieved_payload=retrieved_obj.payload()
    retrieved_format=retrieved_payload['MasterConfigurationKey']
    SuperMasterKey=int(999)
    printfunc ("SMK SuperMasterKey default =",SuperMasterKey)
    SuperMasterKey = int(retrieved_format)
    printfunc ("SMK SuperMasterKey from Cool =",SuperMasterKey)
    coolDB.closeDatabase()

   #from IOVDbSvc.CondDB import conddb
   #conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib")
   #conddb.blockFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels")
   #conddb.blockFolder("/TRIGGER/Receivers/Factors/CalibGains")
   #conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib<tag>HEAD</tag>")
   #conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels<tag>HEAD</tag>")
   #conddb.addFolder("", "<dbConnection>sqlite://;schema=/det/l1calo/calib/tdaq-05/calib.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/Receivers/Factors/CalibGains<tag>HEAD</tag>")


