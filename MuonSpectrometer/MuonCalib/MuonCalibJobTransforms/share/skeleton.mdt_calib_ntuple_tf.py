from RecExConfig.RecFlags import rec as recFlags
recFlags.doFloatingPointException.set_Value_and_Lock(False)

# get the logger
import logging
mdtCalibNtuplelog = logging.getLogger('mdt_calib_ntuple')
mdtCalibNtuplelog.info('************** STARTING MDTCalibNtuple **************')
mdtCalibNtuplelog.info( '**** Transformation run arguments ****' )
mdtCalibNtuplelog.info( str(runArgs) )

#==============================================================
# Job definition parameters:
#==============================================================
# Set defaults for boolean variables in runArgs
# Boolean variables do not exist in runArgs if they are not on the command line
# so check if they exist before using them
#doSim      = runArgs.doSim   #Never used so skip it
useAlign   = True
# If segOnly = True ntuple has no variables filled which is pointless
#segOnly    = False
#moore      = False
#mboy       = False
# If standalone = False all ntuple variables are filled with zeros which is pointless
#standalone = True
doSegmentT0Fit = False
doCSC      = True
rawTgc     = False
cosmicRun  = False
lumiBlockNumberFromCool = False
runNumberFromCool = False

if hasattr(runArgs,'useAlign'):
  useAlign   = runArgs.useAlign
#if hasattr(runArgs,'segOnly'):
#  segOnly    = runArgs.segOnly
#if hasattr(runArgs,'moore'):
#  moore      = runArgs.moore
#if hasattr(runArgs,'nboy'):
#  mboy       = runArgs.mboy
#if hasattr(runArgs,'standalone'):
#  standalone = runArgs.standalone
if hasattr(runArgs,'doCSC'):
  doCSC     = runArgs.doCSC
if hasattr(runArgs,'rawTgc'):
  rawTgc    = runArgs.rawTgc
if hasattr(runArgs,'cosmicRun'):
  cosmicRun = runArgs.cosmicRun
if hasattr(runArgs,'doSegmentT0Fit'):
  doSegmentT0Fit = runArgs.doSegmentT0Fit or cosmicRun
if hasattr(runArgs,'lumiBlockNumberFromCool'):
  lumiBlockNumberFromCool = runArgs.lumiBlockNumberFromCool
if hasattr(runArgs,'runNumberFromCool'):
  runNumberFromCool = runArgs.runNumberFromCool

#==============================================================
# Job Configuration parameters:
#==============================================================
from MuonCalibJobTransforms.MDTCalibNtupleConfig import mdtCalibNtupleConfig
# hardcode UseOracle.py which enables use of Frontier DB replicas.
# As far as I can see you always want to do this.
include("RecJobTransforms/UseOracle.py")
# UseOracle.py used to be loaded here via be commandline argument
#for cf in runArgs.jobConfig:
#    include(cf)
mdtCalibNtuplelog.info( '**** Transformation configuration arguments ****' )
mdtCalibNtuplelog.info( str(mdtCalibNtupleConfig) )

#==============================================================
# Job options file for MDT Calib Ntuple creation
#==============================================================

from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags

globalflags.DataSource.set_Value_and_Lock('data')
# This if is sort of silly since 'data' is the only option.
if globalflags.DataSource() == 'data':
  if cosmicRun:
    jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
    globalflags.DetGeo.set_Value_and_Lock('commis')
  else:   
      globalflags.DetGeo.set_Value_and_Lock('atlas')
  globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.geometryVersion)
  recFlags.doTruth.set_Value_and_Lock(False)

# Check if Bfield is on
from AthenaCommon.BFieldFlags import jobproperties
if runArgs.geometryVersion.find('NF') >=0:    #no field
  jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
  jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
  jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
else:
  jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
  jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
  jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)

# CONDBR2 (Run2 COOL instance) will be selected by default
# If processing Run1 calibstream force COMP200 selection by setting 
# a project name which has the digits "12" so Athena knows this is Run1 
if runArgs.runNumber < 220000:    #Run1
  from RecExConfig.RecFlags import rec    
  rec.projectName="calb12_um"             #Force Run1, COMP200 selection 
from MuonRecExample import MuonRecStandaloneFlags
from MuonRecExample import MuonRecStandaloneOnlySetup
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([])
athenaCommonFlags.RuntimeStrictness.set_Value_and_Lock("none")

# Run on CSC data or not (why would you not want to?)
muonRecFlags.doCSCs=doCSC

# Add rawTgc and rawTgcCoin branches to calib ntuple
muonRecFlags.calibNtupleRawTGC = rawTgc

globalflags.ConditionsTag.set_Value_and_Lock(runArgs.conditionsTag)
from IOVDbSvc.CondDB import conddb
if globalflags.ConditionsTag():
  conddb.setGlobalTag(globalflags.ConditionsTag())
print globalflags

# Turn off detectors which are not present in calibstream
DetFlags.Calo_setOff()
DetFlags.TRT_setOff()
DetFlags.ID_setOff()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# No longer needed, standalone is the only option set True by default.
# ntuple variables will be filled with zero's if it is set False 
# Run Moore
#muonRecFlags.doMoore = moore #### SET VIA COMMAND LINE IN THE TRF
# Run Muonboy
#muonRecFlags.doMuonboy = (mboy or standalone) #### SET VIA COMMAND LINE IN THE TRF
#muonRecFlags.doMuonboy = mboy
#NOTE: In 17.2 the 3rd chain cannot run w/o muonboy.
#turn on/off track reconstruction
#muonRecFlags.doSegmentsOnly = segOnly #### SET VIA COMMAND LINE IN THE TRF
#muonRecFlags.doStandalone = standalone #3rd chain

# for MDT calibration constants
muonRecFlags.dataPeriod.set_Value_and_Lock('LHC')

# Switch for alignment corrections  (on by default)
muonRecFlags.useAlignmentCorrections = useAlign

# calibration ntuple
muonRecFlags.doCalib             = False   
muonRecFlags.doCalibNtuple       = True 
muonRecFlags.calibNtupleOutput   = runArgs.ntupleFile
muonRecFlags.calibNtupleSegments = True
muonRecFlags.calibNtupleTracks   = True

# Switch for T0 segment fit (on by default)
muonRecFlags.doSegmentT0Fit = doSegmentT0Fit 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if muonRecFlags.doMDTs:
  DetFlags.makeRIO.MDT_setOff()
if muonRecFlags.doRPCs:
  DetFlags.makeRIO.RPC_setOn()
if muonRecFlags.doTGCs:
  DetFlags.makeRIO.TGC_setOn()

#Set up cabling which is different for Run1/Run2. 
from MuonMDT_Cabling.MuonMDT_CablingConf import MuonMDT_CablingSvc
if runArgs.runNumber > 220000:    #Run2
  svcMgr += MuonMDT_CablingSvc(name="MuonMDT_CablingSvc", UseOldCabling=False, ForcedUse=False, doCalStreamInit=True)
else:
  svcMgr += MuonMDT_CablingSvc(name="MuonMDT_CablingSvc", UseOldCabling=True, ForcedUse=False, doCalStreamInit=True)

include("MuonRecExample/MuonRDO_to_PRD_jobOptions.py")
   
# Options to select tag for T0/RT or to read from SQLite files.
# If reading from SQLite, you must specify the tag used in the SQLite file
if runArgs.dbT0Tag!='DEFAULT':
  if runArgs.dbT0Sql!='DEFAULT':
    conddb.blockFolder("/MDT/T0BLOB")
    conddb.addFolder(runArgs.dbT0Sql,"/MDT/T0BLOB <tag>"+runArgs.dbT0Tag+"</tag>",True)
  else:
    conddb.addOverride("/MDT/T0BLOB",runArgs.dbT0Tag)
if runArgs.dbRTTag!='DEFAULT':
  if runArgs.dbRTSql!='DEFAULT':
    conddb.blockFolder("/MDT/RTBLOB")
    conddb.addFolder(runArgs.dbRTSql,"/MDT/RTBLOB <tag>"+runArgs.dbRTTag+"</tag>",True)
  else:
    conddb.addOverride("/MDT/RTBLOB",runArgs.dbRTTag)

#  This code disables the ordinary Athena event loop and installs the calibstream event loop
include ("MuonCalibStreamCnvSvc/MuonCalibStream_jobOptions.py")

theApp.EvtMax                                   = runArgs.Events
EventSelectorMuonCalibStream.SkipEvents         = runArgs.Skip
svcMgr.MuonCalibStreamDataProviderSvc.RunNumber = runArgs.runNumber  
svcMgr.MuonCalibStreamFileInputSvc.InputFiles   = runArgs.inputCSFiles

svcMgr.MuonCalibStreamDataProviderSvc.LumiBlockNumberFromCool = lumiBlockNumberFromCool
svcMgr.MuonCalibStreamDataProviderSvc.RunNumberFromCool       = runNumberFromCool

print topSequence

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

svcMgr += CfgMgr.MessageSvc( Format = "% F%50W%S%7W%R%T %0W%M",
                             defaultLimit=1000000, OutputLevel =INFO)

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
