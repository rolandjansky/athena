# get the logger

from RecExConfig.RecFlags import rec as recFlags
recFlags.doFloatingPointException.set_Value_and_Lock(False)

from AthenaCommon.Logging import *
mdtCalibNtuplelog = logging.getLogger('mdtCalibNtuple')

mdtCalibNtuplelog.info('************** STARTING MDT CalibNtuple **************')
#==============================================================
# Job definition parameters:
#==============================================================
EvtMax = runArgs.maxEvents
SkipEvents = runArgs.skipEvents
InputFiles = runArgs.inputBSFile
RunNumber  = runArgs.runNumber
NtupleFile = runArgs.ntupleFile
doSim      = runArgs.doSim
geover     = runArgs.geometryVersion
#new params to be implemented
globalTag  = runArgs.conditionsTag
useAlign   = runArgs.useAlign
dbT0Tag    = runArgs.dbT0Tag
dbRTTag    = runArgs.dbRTTag
dbT0Sql    = runArgs.dbT0Sql
dbRTSql    = runArgs.dbRTSql
segOnly    = runArgs.segOnly
moore      = runArgs.moore
mboy       = runArgs.mboy
standalone = runArgs.standalone
doCSC      = runArgs.doCSC
rawTgc     = runArgs.rawTgc
cosmicRun  = runArgs.cosmicRun
doSegmentT0Fit = runArgs.doSegmentT0Fit or cosmicRun
lumiBlockNumberFromCool = runArgs.lumiBlockNumberFromCool
runNumberFromCool = runArgs.runNumberFromCool

if geover.find('NF') >=0:
  field=False
else:
  field=True

mdtCalibNtuplelog.info( '**** Transformation run arguments ****' )
mdtCalibNtuplelog.info( str(runArgs) )

#==============================================================
# Job Configuration parameters:
#==============================================================
from MuonCalibJobTransforms.MDTCalibNtupleConfig import mdtCalibNtupleConfig
for cf in runArgs.jobConfig:
    include(cf)
mdtCalibNtuplelog.info( '**** Transformation configuration arguments ****' )
mdtCalibNtuplelog.info( str(mdtCalibNtupleConfig) )

#==============================================================
# Job options file for MDT Calib Ntuple creation
#==============================================================


# #######################################################################
# TopOptions to process calibration stream
# #######################################################################

from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags


globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')   #Fabrizio
globalflags.DataSource.set_Value_and_Lock('data')
if globalflags.DataSource() == 'data':
    if cosmicRun:
          jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
          globalflags.DetGeo.set_Value_and_Lock('commis')
    else:
          globalflags.DetGeo.set_Value_and_Lock('atlas')
    globalflags.DetDescrVersion.set_Value_and_Lock(geover)
    recFlags.doTruth.set_Value_and_Lock(False)

from AthenaCommon.BFieldFlags import jobproperties
if field:
 jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
 jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
 jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
else:
 jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
 jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
 jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)


from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.RecFlags import rec    #Ben (avoid Run1, COMP200 selection)
rec.projectName="calb15_um"             #Ben 
from MuonRecExample import MuonRecStandaloneFlags
from MuonRecExample import MuonRecStandaloneOnlySetup
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([])
athenaCommonFlags.RuntimeStrictness.set_Value_and_Lock("none")

# Run on CSC data or not
muonRecFlags.doCSCs=doCSC

globalflags.ConditionsTag.set_Value_and_Lock(globalTag)##### SET VIA COMMAND LINE IN THE TRF
from IOVDbSvc.CondDB import conddb
if globalflags.ConditionsTag():
      conddb.setGlobalTag(globalflags.ConditionsTag())
print globalflags
DetFlags.Calo_setOff()
DetFlags.TRT_setOff()
DetFlags.ID_setOff()
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# Run Moore
muonRecFlags.doMoore = moore #### SET VIA COMMAND LINE IN THE TRF
# Run Muonboy
#muonRecFlags.doMuonboy = (mboy or standalone) #### SET VIA COMMAND LINE IN THE TRF
muonRecFlags.doMuonboy = mboy
#NOTE: In 17.2 the 3rd chain cannot run w/o muonboy.
#turn on/off track reconstruction
muonRecFlags.doSegmentsOnly = segOnly #### SET VIA COMMAND LINE IN THE TRF
muonRecFlags.doStandalone = standalone #3rd chain


# for MDT calibration constants
muonRecFlags.dataPeriod.set_Value_and_Lock('LHC')

# Switch on the alignment corrections
muonRecFlags.useAlignmentCorrections = useAlign##### SET VIA COMMAND LINE IN THE TRF

# calibration ntuple
muonRecFlags.doCalib = False 
muonRecFlags.doCalibNtuple = True 
muonRecFlags.calibNtupleOutput=NtupleFile ##### SET VIA COMMAND LINE IN THE TRF
muonRecFlags.calibNtupleSegments = True
muonRecFlags.calibNtupleTracks = True
muonRecFlags.calibNtupleRawTGC = rawTgc ##### SET VIA COMMAND LINE IN THE TRF

# Switch off the T0 fit (on by default for data)
#muonRecFlags.doSegmentT0Fit = False
muonRecFlags.doSegmentT0Fit = doSegmentT0Fit 

topSequence = AlgSequence()
if muonRecFlags.doMDTs:
       DetFlags.makeRIO.MDT_setOff()
if muonRecFlags.doRPCs:
       DetFlags.makeRIO.RPC_setOn()
if muonRecFlags.doTGCs:
       DetFlags.makeRIO.TGC_setOn()

#Set up new cabling. 
from MuonMDT_Cabling.MuonMDT_CablingConf import MuonMDT_CablingSvc
#svcMgr += MuonMDT_CablingSvc(name="MuonMDT_CablingSvc", false , false)
#svcMgr.MuonMDT_CablingSvc.doCalStreamInit=True
svcMgr += MuonMDT_CablingSvc(name="MuonMDT_CablingSvc", UseOldCabling=False, ForcedUse=False, doCalStreamInit=True)
#svcMgr.MuonMDT_CablingSvc.doCalStreamInit=True
print 'skeleton: setting doCalStreamInit true'

include("MuonRecExample/MuonRDO_to_PRD_jobOptions.py")
   
# to alter DB options
#overwriting the tag
if dbT0Tag!='DEFAULT':
  if dbT0Sql!='DEFAULT':
    conddb.blockFolder("/MDT/T0BLOB")
    conddb.addFolder(dbT0Sql,"/MDT/T0BLOB <tag>"+dbT0Tag+"</tag>",True)
  else:
    conddb.addOverride("/MDT/T0BLOB",dbT0Tag)
if dbRTTag!='DEFAULT':
  if dbRTSql!='DEFAULT':
    conddb.blockFolder("/MDT/RTBLOB")
    conddb.addFolder(dbRTSql,"/MDT/RTBLOB <tag>"+dbRTTag+"</tag>",True)
  else:
    conddb.addOverride("/MDT/RTBLOB",dbRTTag)

include ("MuonCalibStreamCnvSvc/MuonCalibStream_jobOptions.py")
theApp.EvtMax = EvtMax                ##### SET VIA COMMAND LINE IN THE TRF
EventSelectorMuonCalibStream.SkipEvents=SkipEvents  ##### SET VIA COMMAND LINE IN THE TRF
svcMgr.MuonCalibStreamDataProviderSvc.RunNumber=RunNumber ##### SET VIA COMMAND LINE IN THE TRF
svcMgr.MuonCalibStreamFileInputSvc.InputFiles = InputFiles ##### SET VIA COMMAND LINE IN THE TRF

svcMgr.MuonCalibStreamDataProviderSvc.LumiBlockNumberFromCool=lumiBlockNumberFromCool
svcMgr.MuonCalibStreamDataProviderSvc.RunNumberFromCool=runNumberFromCool

print topSequence

# obselete
#include ("BFieldAth/BFieldAth_jobOptions.py")
import MagFieldServices.SetupField

# lock all flags. Very important!
muonRecFlags.lock_JobProperties()

# Enable conversion of old eventInfo to xAOD eventInfo (Dongliang)
from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg('EventInfoCnvAlg')
topSequence.EventInfoCnvAlg.AODKey='MuonCalibStreamEventInfo'

include ("MuonRecExample/MuonRec_jobOptions.py")
try:
	ToolSvc.CscSegmentUtilTool.Add2hitSegments = True
except AttributeError:
	pass

#ToolSvc.CscSegmentUtilTool.OutputLevel = VERBOSE
#ToolSvc.MooTrackSteering.DumpCombinatorics = True
svcMgr += CfgMgr.MessageSvc( Format = "% F%50W%S%7W%R%T %0W%M",
                                 defaultLimit=1000000, OutputLevel =INFO)

#if not mboy and not segOnly and hasattr(topSequence.MuonCalibExtraTreeAlg, "Track2MELocation"):
#        topSequence.MuonCalibExtraTreeAlg.Track2MELocation=""
  
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
