#==============================================================
#
# Job options file
#
# @file TrigT1CTMonitoringJobOptions.py
#
# @brief Monitoring for LVL1 Central Trigger bytestream.
#
# @author David Berge <berge@cern.ch>
#         Eleanor Dobson <eleanor.dobson@cern.ch>
#         Christian Ohm <christian.ohm@cern.ch>
#==============================================================

#function to get keys from the trigger database
from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
def _getKeys(run,lb):
    runlist = [[run,run]]
    dbconn  = TriggerCoolUtil.GetConnection("COMP",0)

    config  = TriggerCoolUtil.getHLTConfigKeys(dbconn, runlist)
    l1keys  = TriggerCoolUtil.getL1ConfigKeys(dbconn, runlist)
    hltkeys = TriggerCoolUtil.getHLTPrescaleKeys(dbconn, runlist)
    
    smk=config[run]['SMK']
    
    for k,b,e in l1keys[run]['LVL1PSK']:
        if b>lb: break
        l1psk=k

    for k,b,e in hltkeys[run]['HLTPSK2']:
        if b>lb: break
        hltpsk=k

    return (smk,l1psk,hltpsk)

def _getBunchGroupKey(run,lb):
    runlist = [[run,run]]
    dbconn  = TriggerCoolUtil.GetConnection("COMP",0)

    bGroupkeys = TriggerCoolUtil.getBunchGroupKey(dbconn, runlist)
    print bGroupkeys[run]['BGKey']
    
    for k,b,e in bGroupkeys[run]['BGKey']:
        if b>lb: break
        bGroup=k

    return bGroup


whoami = os.popen('whoami').read().rstrip()

pathToFiles = "/afs/cern.ch/user/p/pottgen/212619_LB4_missing_orbit/"
#pathToFiles = "/afs/cern.ch/work/c/cohm/L1CT/data/218006_missing_CTP_RIO/"
#pathToFiles = "/afs/cern.ch/work/c/cohm/L1CT/data/213964_LB72_MIOCT_problem/"
#pathToFiles = "/tmp/cohm/"
#pathToFiles = "/afs/cern.ch/work/c/cohm/L1CT/data/213964_LB72_MIOCT_problem/"
#pathToFiles = "/afs/cern.ch/work/"+whoami[0]+"/"+whoami+"/L1CT/data/208484_LB95_sim_mismatch/"
#pathToFiles = "/afs/cern.ch/work/"+whoami[0]+"/"+whoami+"/L1CT/data/212858_LB730_missing_fragment/"
#pathToFiles = "/afs/cern.ch/work/"+whoami[0]+"/"+whoami+"/L1CT/data/213311_lb097_missing/"
#pathToFiles = "/afs/cern.ch/work/"+whoami[0]+"/"+whoami+"/L1CT/data/L1MU_error_hunting_only_problematic/"

files = [pathToFiles+f for f in os.listdir(pathToFiles)]

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(files) #for local running
#athenaCommonFlags.FilesInput.set_Value_and_Lock([ "/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data" ]) # for grid running

from RecExConfig.RecFlags import rec #otherwise configgetter tries to set up output
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = DEBUG
MessageSvc.defaultLimit = 10000000

## FILEKEY must match that given to THistSvc

if (not "doPT" in dir()):
    doPT=False
if (not "doMT" in dir()):
    doMT=False

# when run with athenaPT, a different THistSvc is needed that when running with PT only
if (not "doAthenaPT" in dir()):
    doAthenaPT=False

onlineMode = False
if (doMT or doPT):
    onlineMode = True

#--------------------------------------------------------------
# 
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DataSource.set_Value_and_Lock('bytestream')
globalflags.DataSource.set_Value_and_Lock('atlas')
IsData=False
if globalflags.DataSource()=='data' :
    IsData=True

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.geometry.all_setOn()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Truth_setOff()
DetFlags.Muon_setOn()
DetFlags.LVL1_setOn()
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.digitize.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.writeRDOPool.all_setOff()
DetFlags.readRDOPool.RPC_setOn()
DetFlags.readRDOPool.TGC_setOn()
DetFlags.readRDOBS.LVL1_setOn()
DetFlags.readRIOBS.LVL1_setOn()
DetFlags.Print()

#--------------------------------------------------------------
# GeoModel stuff:
#--------------------------------------------------------------
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

#--------------------------------------------------------------
# 
#--------------------------------------------------------------
include( "AmdcAth/AmdcAth_jobOptions.py" )
include( "AtlasGeoModel/MuonGeoModel.py" )

# Cabling and geometry of muon detectors
from MuonCnvExample.MuonCnvFlags import muonCnvFlags
muonCnvFlags.RpcCablingMode.set_Value_and_Lock( "new" )
muonCnvFlags.TgcCablingMode.set_Value_and_Lock( "12-fold" )
# Needed to decode the RoI information
from TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig import RPCRecRoiConfig
from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig import TGCRecRoiConfig

from PyUtils.MetaReaderPeeker import metadata
from TrigT1CTMonitoring.TrigT1CTMonitoringConfig import *

#below to read from cool
from IOVDbSvc.CondDB import conddb
#conddb.addFolder('TRIGGER', "/TRIGGER/LVL1/BunchGroupContent") # seems to not work, and not be needed!
conddb.addFolder('DCS_OFL', "/LHC/DCS/FILLSTATE")
conddb.addFolder('TDAQ',    "/TDAQ/RunCtrl/DataTakingMode")
conddb.addFolder('TRIGGER', "/TRIGGER/LVL1/RFPhase")
conddb.addFolder('TRIGGER', '/TRIGGER/LVL1/CTPCoreInputMapping')
conddb.setGlobalTag('COMCOND-REPPST-006-02')
#print "globalflags.ConditionsTag():", globalflags.ConditionsTag()
#conddb.setGlobalTag(globalflags.ConditionsTag())

#---------------------------------------------------------------
# Load Trigger configuration from DB using cool data keys
# load algorithms to run CTP simulation on rederived inputs
#---------------------------------------------------------------
from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
LVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')
svcMgr += LVL1ConfigSvc

testLabFile=False
RunSimOnData=True
checkDQ=False

if testLabFile:
    svcMgr.IOVDbSvc.forceRunNumber = 167844
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerConfig="LVL1:DB:TRIGGERDB:833,2010"
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()
    LVL1ConfigSvc.DBBGSKey=11

elif RunSimOnData:
    run = metadata['runNumbers'][0]
    lb = metadata['lumiBlockNumbers'][0]
    keys = _getKeys(run,lb)
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerConfig="DATARECO:DB:TRIGGERDB:%i,%i,%i" % keys
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()
    BGSet=_getBunchGroupKey(run, lb)
    LVL1ConfigSvc.DBBGSKey=BGSet
    
    #Run L1Calo simulation
    #include ( "TrigT1CaloSim/TrigT1CaloSimJobOptions_ReadTT.py" )

    #rederive MuCTPI inputs to CTP from muon RDO
    #writes this to the usual MuCTPICTP storegate location

    from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_Data
    topSequence += L1Muctpi_on_Data()
    
    from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__DeriveSimulationInputs as DeriveSimulationInputs
 
    topSequence += DeriveSimulationInputs(do_MuCTPI_input=True,
                                          do_L1Calo_sim=False)
        
    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationOnData
    topSequence += CTPSimulationOnData("CTPSimulation")
    
elif checkDQ:
    run = metadata['runNumbers'][0]
    lb = metadata['lumiBlockNumbers'][0]
    keys = _getKeys(run,lb)
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerConfig="DATARECO:DB:TRIGGERDB:%i,%i,%i" % keys
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()
    BGSet=_getBunchGroupKey(run, lb)
    LVL1ConfigSvc.DBBGSKey=BGSet

else:
    LVL1ConfigSvc.ConfigSource = "XML"
    #LVL1ConfigSvc.XMLFile = "HLTconfig_Physics_pp_v1_16.1.0.2.xml"
    LVL1ConfigSvc.XMLMenuFile = "HLTconfig_Physics_pp_v1_16.1.0.2.xml"
    LVL1ConfigSvc.CreateLegacyObjects = True
    svcMgr += LVL1ConfigSvc
    
## get a handle on the ToolSvc
## add an AthenaMonManager algorithm to the list of algorithms to be ran
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if not hasattr(topSequence,"PrimaryManager"):
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    topSequence += AthenaMonManager( "PrimaryManager" )
## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
CTmonMan = topSequence.PrimaryManager

from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__BSMonitoring as BSMon
#svcMgr.ToolSvc += BSMon()
BSMonJob=BSMon(ProcessMuctpiData=True,
               ProcessMuctpiDataRIO=True,
               ProcessCTPData=True,
               ProcessRoIBResult=True,
               InclusiveTriggerThresholds=True,
               CompareRerun=True)

CTmonMan.AthenaMonTools += [BSMonJob] #[ "TrigT1CTMonitoring__BSMonitoring/BSMon" ]
CTmonMan.FileKey = "GLOBAL"
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = [ CTmonMan.FileKey + " DATAFILE='HistFile.root' OPT='RECREATE'"];


include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr.EventSelector.Input = athenaCommonFlags.FilesInput()
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )

#---------to read L1Calo bytestream----------------------------
include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
include ("TrigT1CaloMonitoring/TrigT1CaloMonitoring_LoadBSConverter.py")

if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "MuCTPI_RDO/MUCTPI_RDO",
    "CTP_RDO/CTP_RDO",
    "MuCTPI_RIO/MUCTPI_RIO",
    "CTP_RIO/CTP_RIO"
]

print topSequence
print svcMgr

theApp.EvtMax = -1
