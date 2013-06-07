# get the logger
from AthenaCommon.Logging import *
rpcCalibNtuplelog = logging.getLogger('rpcCalibNtuple')

rpcCalibNtuplelog.info('************** STARTING RPC CalibNtuple **************')
#==============================================================
# Job definition parameters:
#==============================================================

EvtMax = runArgs.maxEvents
SkipEvents = runArgs.skipEvents
InputFiles = runArgs.inputESDFile
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

#==============================================================
# Job Configuration parameters:
#==============================================================
from MuonCalibJobTransforms.RPCCalibNtupleConfig import rpcCalibNtupleConfig
for cf in runArgs.jobConfig:
    include(cf)
rpcCalibNtuplelog.info( '**** Transformation configuration arguments ****' )
rpcCalibNtuplelog.info( str(rpcCalibNtupleConfig) )

rpcClusterTimeSpread=rpcCalibNtupleConfig.clusterTimeSpread
doSim=rpcCalibNtupleConfig.doSim



from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BFieldFlags import jobproperties



from AthenaCommon.DetFlags import DetFlags

topSequence = AlgSequence()

#####################
## common/global config
#####################


DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()

#Minimally required flags for commissioning
#include("RecExCommission/MinimalCommissioningSetup.py")

athenaCommonFlags.PoolESDInput.set_Value_and_Lock(InputFiles)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(SkipEvents)
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)

globalflags.DetDescrVersion.set_Value_and_Lock(GeoVersion)
globalflags.ConditionsTag.set_Value_and_Lock(condTag)

# field
fieldFlag=[bool(int(FieldMask[0])),bool(int(FieldMask[1])),bool(int(FieldMask[2]))]

jobproperties.BField.solenoidOn.set_Value_and_Lock(fieldFlag[0])
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(fieldFlag[1])
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(fieldFlag[2])


#####################
## rec config
#####################

rec.doJetMissingETTag.set_Value_and_Lock(False)
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled.set_Value_and_Lock(False)
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
recAlgs.doMuonSpShower.set_Value_and_Lock(False)

rec.doHist.set_Value_and_Lock(False)
# don't want CBNT from RecExCommon. Will config it ourselves
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)	
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)

rec.readESD.set_Value_and_Lock(True)
rec.doESD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
globalflags.InputFormat.set_Value_and_Lock('pool')

###################
## specific config
###################

# add RPC clusterization at the beginning

from RpcClusterization.RpcClusterizationConf import RpcClusterBuilderPRD
topSequence+=RpcClusterBuilderPRD(ClusterTimeSpread = 10000, OutputLevel = FATAL)

#--------------------------------------------------------------
# ntuple config  
#--------------------------------------------------------------
theApp.HistogramPersistency = "ROOT"

THistSvc = CfgMgr.THistSvc()
ServiceMgr += THistSvc
outputString="AANT DATAFILE=\'"+NtupleFile+"\' OPT=\'RECREATE\'"
THistSvc.Output = [outputString]
topSequence += CfgMgr.AANTupleStream(ExtraRefNames = [ "" ],
                                     OutputName = NtupleFile,
                                     ExistDataHeader = False )

include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )             
from CBNT_Athena.CBNT_AthenaConf import CBNTAA_Audit
CBNT_AthenaAware += CBNTAA_Audit()

# activate RPC-specific ntuples

if(extrnt):
    include('RpcCalibAlgs/RpcExtrapolNtuple_jobOptions.py' )

if(resnt):
    include('RpcCalibAlgs/RpcResidualsNtuple_jobOptions.py')
    if recoalg=='Moore':
        CBNT_AthenaAware.RpcResidualsNtuple.TrackCollection = "MooreTracks"
if(prdnt):
    
    CBNT_AthenaAware.RpcPrepRawDataNtuple.Hit=False
    CBNT_AthenaAware.RpcPrepRawDataNtuple.Enable=True
else:
    try:
        CBNT_AthenaAware.RpcPrepRawDataNtuple.Enable=False
    except AttributeError:
        pass
    

#load muon rec tools
include ("MuonRecExample/MuonRecLoadTools.py")


# the main jobOpt
include("RecExCommission/RecExCommission.py")

include ("RecExCommon/RecExCommon_topOptions.py")


