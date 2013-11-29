from RecExConfig.RecFlags import rec as recFlags

# # Temporary
# from AthenaCommon.BeamFlags import jobproperties
# # various configuration special to commissioning, needed for all jobs
# jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
# 
# #globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-04-00-00')
# globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-003-00')
# # for running over data with field on need this to be setto true
# from AthenaCommon.BFieldFlags import jobproperties
# jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
# jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
# jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
# # End temporary

# steering file for ESD->ESD step
# see myTopOptions.py for more info
doCBNT=False
readESD=True
doWriteESD=True # uncomment if do not write ESD
doWriteAOD=False # uncomment if do not write AOD
doAOD=False # uncomment if do not run AOD making algorithms
doWriteTAG=False # uncomment if do not write TAG
EvtMax=-1

from MuonRecExample import MuonRecUtils
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

### choose one of the pre-defined data files from DataFiles.py
if not MuonRecUtils.hasJobPropertyBeenSet(athenaCommonFlags.FilesInput):
    athenaCommonFlags.FilesInput = ["ESD.pool.root"]

#if not 'PoolESDInput' in dir():
#    PoolESDInput=["ESD.pool.root"]
if not 'PoolESDOutput' in dir():
    PoolESDOutput="copy_ESD.pool.root"
# main jobOption

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
#DetFlags.ID_setOff()
DetFlags.Calo_setOff()
# DetFlags.Muon_setOff()
muonRecFlags.doCalib=False

# No PRD to RDO
DetFlags.makeRIO.all_setOff()
# Run Moore
muonRecFlags.doMoore = False
# Run Muonboy
muonRecFlags.doMuonboy = False

# Want to use this for position checks
# recFlags.doCBNT = True

# Force it to be old cabling:
from MuonCnvExample.MuonCnvFlags import muonCnvFlags
muonCnvFlags.TgcCablingMode.set_Value_and_Lock('12-fold')
# I've manually added RDOs to these POOL files
DetFlags.readRDOPool.Muon_setOn()
import MuonCnvExample.MuonCablingConfig


include ("RecExCommon/RecExCommon_topOptions.py")


# ------------------------------------------------------------
# TEST EDM
# ------------------------------------------------------------

if not 'DumpFileName' in dir():
    DumpFileName="ReadESD"

include ("MuonTestEDM/MuonTestEDM_jobOptions.py")
#MyEDMTester.OutputLevel=VERBOSE
MyEDMTester.DoDetailedChecks=True

print MyEDMTester

from TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
TrkEventCnvSuperTool = Trk__EventCnvSuperTool(name="EventCnvSuperTool", DoID=False)
ToolSvc += TrkEventCnvSuperTool
print TrkEventCnvSuperTool

