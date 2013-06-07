# get the logger
from AthenaCommon.Logging import *
rpcCalibNtuplelog = logging.getLogger('rpcCalibNtuple')

rpcCalibNtuplelog.info('************** STARTING RPC CalibNtuple **************')
#==============================================================
# Job definition parameters:
#==============================================================
EvtMax = runArgs.maxEvents
SkipEvents = runArgs.skipEvents
InputFiles = runArgs.inputBSFile
RunNumber  = runArgs.runNumber
NtupleFile = runArgs.ntupleFile
FieldMask  = runArgs.fieldMask
condTag    = runArgs.conditionsTag
trignt     = runArgs.trigNtuple
resnt      = runArgs.resNtuple
prdnt      = runArgs.prdNtuple
extrnt     = runArgs.extrNtuple
recoalg    = runArgs.recoAlg

GeoVersion=runArgs.geometryVersion

rpcCalibNtuplelog.info( '**** Transformation run arguments ****' )
rpcCalibNtuplelog.info( str(runArgs) )

from RecExConfig.RecFlags import rec as recFlags
recFlags.doFloatingPointException.set_Value_and_Lock(False)


#==============================================================
# Job Configuration parameters:
#==============================================================

include("RecJobTransforms/UseOracle.py")


from MuonCalibJobTransforms.RPCCalibNtupleConfig import rpcCalibNtupleConfig
for cf in runArgs.jobConfig:
    include(cf)
rpcCalibNtuplelog.info( '**** Transformation configuration arguments ****' )
rpcCalibNtuplelog.info( str(rpcCalibNtupleConfig) )

rpcClusterTimeSpread=rpcCalibNtupleConfig.clusterTimeSpread

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc

from RecExConfig.RecFlags import rec
from RecExConfig.RecConfFlags import recConfFlags

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

# For Trigger MBTS and L1 Calo we need the Tile Calorimeter
# the algorithm: MuonCalibExtraTreeTriggerAlg stores trigger info
#DetFlags.makeRIO.Calo_setOn()

#DetFlags.detdescr.LVL1_setOn()
#DetFlags.readRDOBS.LVL1_setOn()

#DetFlags.Calo_setOff()

# Max number of events to run, number of events to skip
#athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)
#athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)

theApp.EvtMax = EvtMax 

# input BS file
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([])
athenaCommonFlags.RuntimeStrictness.set_Value_and_Lock("none")

# Set properly beam type, DataSource, InputFormat, Detector Description, Conditions Tag
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.DetDescrVersion.set_Value_and_Lock(GeoVersion)
globalflags.ConditionsTag.set_Value_and_Lock(condTag)

# field ++++ uncomment if you want to set them by hand +++++
fieldFlag=[bool(int(FieldMask[0])),bool(int(FieldMask[1])),bool(int(FieldMask[2]))]

jobproperties.BField.solenoidOn.set_Value_and_Lock(fieldFlag[0])
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(fieldFlag[1])
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(fieldFlag[2])

from MuonRecExample import MuonRecStandaloneFlags
from MuonRecExample import MuonRecStandaloneOnlySetup
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.DetFlags import DetFlags

from IOVDbSvc.CondDB import conddb
if globalflags.ConditionsTag():
      conddb.setGlobalTag(globalflags.ConditionsTag())


from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


topSequence = AlgSequence()


#####################################################

# Start at formation of PRDs only
#muonRecFlags.Enabled = False

# Disable MDTs +++ set to True also to permit the track reconstruction algos to run +++
muonRecFlags.doMDTs=True

# Disable RPCs
muonRecFlags.doRPCs=True

# Disable TGCs
muonRecFlags.doTGCs=False

# Run on CSC data or not
muonRecFlags.doCSCs.set_Value_and_Lock(False)

muonRecFlags.doCalib.set_Value_and_Lock(False)

muonRecFlags.doCalibNtuple.set_Value_and_Lock(False)

# Stop Muonboy at segment level
#muonboyFlags.doSegmentsOnly = False

# Stop Moore at segment level
#mooreFlags.doSegmentsOnly = False

muonRecFlags.doSegmentsOnly = False

muonRecFlags.dataPeriod.set_Value_and_Lock('LHC')

if(trignt or prdnt):
    muonRecFlags.doRPCs = True

if recoalg=='Moore':
    # Run Moore
    muonRecFlags.doMoore = True
else:
    muonRecFlags.doMoore = False

if recoalg=='MuonBoy':
    # Run Muonboy
    muonRecFlags.doMuonboy = True
else:
    muonRecFlags.doMuonboy = False


if muonRecFlags.doMDTs:
       DetFlags.makeRIO.MDT_setOff()
if muonRecFlags.doRPCs:
       DetFlags.makeRIO.RPC_setOn()
if muonRecFlags.doTGCs:
       DetFlags.makeRIO.TGC_setOn()


# for MDT calibration constants
muonRecFlags.dataPeriod.set_Value_and_Lock('LHC')

include("MuonRecExample/MuonRDO_to_PRD_jobOptions.py")


############
## specific tuning for calib stream
############

include ("MuonCalibStreamCnvSvc/MuonCalibStream_jobOptions.py")
svcMgr.MuonCalibStreamDataProviderSvc.RunNumber=RunNumber
svcMgr.MuonCalibStreamFileInputSvc.InputFiles = InputFiles ##### SET VIA COMMAND LINE IN THE TRF
EventSelectorMuonCalibStream.SkipEvents=SkipEvents

include ("BFieldAth/BFieldAth_jobOptions.py")

# lock all flags. Very important!
muonRecFlags.lock_JobProperties()

#rec.readRDO.set_Value_and_Lock(False)


### General Reco flags jobproperties
#from RecExConfig.RecFlags import rec
##
### no truth when running on data
#rec.doTruth = False
#
#rec.doMuon=True
#
##
### Switch Perfmon studies
#rec.doPerfMon = False
#rec.doDetailedPerfMon = False
#rec.doNameAuditor = False
#
#
#muonRecFlags.lock_JobProperties()
#rec.lock_JobProperties()
#
include ("MuonRecExample/MuonRec_jobOptions.py")

include("MuonRecExample/MuonCBNT_jobOptions.py")


#include ("RecExCommon/RecExCommon_topOptions.py")

#from CBNT_Athena.CBNT_AthenaConf import CBNT_AthenaAware 
#topSequence += CBNT_AthenaAware()

THistSvc = CfgMgr.THistSvc()
ServiceMgr += THistSvc
outputString="AANT DATAFILE=\'"+NtupleFile+"\' OPT=\'RECREATE\'"
THistSvc.Output = [outputString]

try:
    topSequence.AANTupleStream.OutputName=NtupleFile

except AttributeError:
    topSequence += CfgMgr.AANTupleStream(ExtraRefNames = [ "" ],
                                         OutputName = NtupleFile,
                                         ExistDataHeader = False )


# activate RPC-specific ntuples

if(extrnt):
    include('RpcCalibAlgs/RpcExtrapolNtuple_jobOptions.py' )

if(resnt):
    include('RpcCalibAlgs/RpcResidualsNtuple_jobOptions.py')
    if recoalg=='Moore':
        topSequence.CBNT_AthenaAware.RpcResidualsNtuple.TrackCollection = "MooreTracks"

if(prdnt):
    
    topSequence.CBNT_AthenaAware.RpcPrepRawDataNtuple.Hit=True
    topSequence.CBNT_AthenaAware.RpcPrepRawDataNtuple.Enable=True
else:
    try:
        topSequence.CBNT_AthenaAware.RpcPrepRawDataNtuple.Enable=False
    except AttributeError:
        pass
    
if(trignt):
    include('MuonCalibJobTransforms/rpc_trigNtuple.py')
else:
    try:
        topSequence.CBNT_AthenaAware.RpcRawDataNtuple.Enable=False
    except AttributeError:
        pass
try:
    topSequence.CBNT_AthenaAware.MdtPrepRawDataNtuple.Enable=False
except AttributeError:
    pass
try:
    topSequence.CBNT_AthenaAware.MdtRawDataNtuple.Enable=False
except AttributeError:
    pass
try:
    topSequence.CBNT_AthenaAware.CBNTAA_Muon.Enable=False
except AttributeError:
    pass
try:
    topSequence.CBNT_AthenaAware.CBNTAA_Audit.Enable=False
except AttributeError:
    pass
try:
    topSequence.CBNT_AthenaAware.RpcRawDataNtuple.Enable=False
except AttributeError:
    pass
    
ServiceMgr.MessageSvc.infoLimit=10000000
#ServiceMgr.MessageSvc.OutputLevel  = DEBUG
