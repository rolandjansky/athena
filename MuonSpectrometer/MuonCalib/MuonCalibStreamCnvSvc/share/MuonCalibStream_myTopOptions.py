# #######################################################################
# TopOptions to process calibration stream
# #######################################################################

from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags

globalflags.DataSource.set_Value_and_Lock('data')
if globalflags.DataSource() == 'data':
    jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-04-00-00')
    globalflags.DetGeo.set_Value_and_Lock('commis')

from AthenaCommon.BFieldFlags import jobproperties
# for running over data with field on need this to be set to true
jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)


from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.RecFlags import rec
from MuonRecExample import MuonRecStandaloneFlags
from MuonRecExample import MuonRecStandaloneOnlySetup
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.DetFlags import DetFlags

# Run on CSC data or not
muonRecFlags.doCSCs=False

globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-03')
from IOVDbSvc.CondDB import conddb
if globalflags.ConditionsTag():
      conddb.setGlobalTag(globalflags.ConditionsTag())
print globalflags

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# Run Moore
muonRecFlags.doMoore = True
# Run Muonboy
muonRecFlags.doMuonboy = True

# for MDT calibration constants
muonRecFlags.dataPeriod.set_Value_and_Lock('LHC')

# Switch on the alignment corrections
muonRecFlags.useAlignmentCorrections = True
muonRecFlags.doCalib = True 

# Switch off the T0 fit (on by default for data)
#muonRecFlags.doSegmentT0Fit = False

topSequence = AlgSequence()
if muonRecFlags.doMDTs:
       DetFlags.makeRIO.MDT_setOff()
if muonRecFlags.doRPCs:
       DetFlags.makeRIO.RPC_setOn()
if muonRecFlags.doTGCs:
       DetFlags.makeRIO.TGC_setOn()

include("MuonRecExample/MuonRDO_to_PRD_jobOptions.py")
   
include ("MuonCalibStreamCnvSvc/MuonCalibStream_jobOptions.py")
theApp.EvtMax = 100
svcMgr.MuonCalibStreamFileInputSvc.InputFiles = ['/afs/cern.ch/user/d/domizia/project/data/data08_calib.000091060.calibration_muon_all.daq.RAW._0001.data']
svcMgr.MuonCalibStreamDataProviderSvc.RunNumber=91060

print topSequence

include( "BFieldAth/BFieldAth_jobOptions.py" )


# lock all flags. Very important!
muonRecFlags.lock_JobProperties()

include ("MuonRecExample/MuonRec_jobOptions.py")
 

svcMgr += CfgMgr.MessageSvc( Format = "% F%50W%S%7W%R%T %0W%M",
                                 defaultLimit=1000000, OutputLevel =INFO)

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
