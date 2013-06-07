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

#==============================================================
# Job Configuration parameters:
#==============================================================
from MuonCalibJobTransforms.RPCCalibNtupleConfig import rpcCalibNtupleConfig
for cf in runArgs.jobConfig:
    include(cf)
rpcCalibNtuplelog.info( '**** Transformation configuration arguments ****' )
rpcCalibNtuplelog.info( str(rpcCalibNtupleConfig) )

rpcClusterTimeSpread=rpcCalibNtupleConfig.clusterTimeSpread

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from AthenaCommon import CfgMgr

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.DetFlags import DetFlags

from RecExConfig.RecFlags import rec as recFlags

from MuonRecExample.MuonCBNTFlags import muonCBNT
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonCalibAlgs.MuonCalibFlags import muonCalibFlags

from MuonRecExample import MuonRecStandaloneOnlySetup

topSequence = AlgSequence()


#####################
## common/global config
#####################

# For Trigger MBTS and L1 Calo we need the Tile Calorimeter
# the algorithm: MuonCalibExtraTreeTriggerAlg stores trigger info
DetFlags.makeRIO.Calo_setOn()
DetFlags.detdescr.LVL1_setOn()
DetFlags.readRDOBS.LVL1_setOn()

# Max number of events to run, number of events to skip
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(SkipEvents)

athenaCommonFlags.BSRDOInput.set_Value_and_Lock(InputFiles)

globalflags.DataSource.set_Value_and_Lock('data')
jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DetDescrVersion.set_Value_and_Lock(GeoVersion)
globalflags.ConditionsTag.set_Value_and_Lock(condTag)

# field
fieldFlag=[bool(int(FieldMask[0])),bool(int(FieldMask[1])),bool(int(FieldMask[2]))]

jobproperties.BField.solenoidOn.set_Value_and_Lock(fieldFlag[0])
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(fieldFlag[1])
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(fieldFlag[2])

############################
## muon specific
############################

# we want tracks, not just segments
muonRecFlags.doSegmentsOnly = False

################################################################################
# Set your user flags here
################################################################################

muonCBNT.doMdtRaw = False
muonCBNT.doRpcRaw = False
muonCBNT.doCscRaw = False
muonCBNT.doTgcRaw = False
muonCBNT.doMuonboy = False
muonCBNT.doMoore = False

if(trignt or prdnt):
    muonCBNT.doRpcRaw = True

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

# Run on CSC data or not
muonRecFlags.doCSCs=False

# Switch on the alignment corrections
muonRecFlags.useAlignmentCorrections = True
muonRecFlags.doCalib = False

recFlags.doTruth = False

#rec.doCBNT = True
recFlags.doESD.set_Value_and_Lock( False )
recFlags.doWriteESD.set_Value_and_Lock( False )
recFlags.Commissioning = True

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.03.02.Initial.Light.BML.S13.Patch"

# the following two lines are needed to remove annoying file ESD.pool.root
include("RecExCommission/RecExCommissionFlags_jobOptions.py")
ATLASCosmicFlags.doFilteredESD=False

recFlags.lock_JobProperties()
muonRecFlags.lock_JobProperties()


# The main jobOption file
include("RecExCommission/RecExCommission.py")
include ("RecExCommon/RecExCommon_topOptions.py")

# Tune RPC clusterization

CfgMgr.RpcClusterBuilderPRD.ClusterTimeSpread = 10000
CfgMgr.RpcClusterBuilderPRD.OutputLevel = FATAL

##--------------------------------------------------------------
## Ntuple configuration
##--------------------------------------------------------------
#theApp.HistogramPersistency = "ROOT"
#
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
        CBNT_AthenaAware.RpcResidualsNtuple.TrackCollection = "MooreTracks"
if(prdnt):
    
    CBNT_AthenaAware.RpcPrepRawDataNtuple.Hit=False
    CBNT_AthenaAware.RpcPrepRawDataNtuple.Enable=True
else:
    try:
        CBNT_AthenaAware.RpcPrepRawDataNtuple.Enable=False
    except AttributeError:
        pass
    
if(trignt):
    include('MuonCalibJobTransforms/rpc_trigNtuple.py')
else:
    try:
        CBNT_AthenaAware.RpcRawDataNtuple.Enable=False
    except AttributeError:
        pass

#CBNT_AthenaAware.TgcRawDataNtuple.Enable=False
#CBNT_AthenaAware.RpcRawDataNtuple.Enable=False
#CBNT_AthenaAware.MdtRawDataNtuple.Enable=False
#CBNT_AthenaAware.MdtPrepRawDataNtuple.Enable=False
#CBNT_AthenaAware.RpcPrepRawDataNtuple.Enable=True
#CBNT_AthenaAware.RpcPrepRawDataNtuple.Hit=False
#CBNT_AthenaAware.CBNTAA_Muon.Enable=True
#CBNT_AthenaAware.MuonMakeTrackNtuple.Enable=False
#CBNT_AthenaAware.MuonMakeSegmentNtuple.Enable=False

#if(resnt):
#    topSequence.AANTupleStream.FilterAlgs+=['RpcResidualsNtuple/RpcResidualsNtuple']
