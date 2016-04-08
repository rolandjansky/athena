from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

#Minimally required flags for commissioning
include("RecExCommission/MinimalCommissioningSetup.py")

athenaCommonFlags.FilesInput=["TAG.root"]
rec.Commissioning.set_Value_and_Lock(True)
# choose what detector to turn on:
rec.doInDet.set_Value_and_Lock(True)
rec.doLArg.set_Value_and_Lock(True)
rec.doTile.set_Value_and_Lock(True)
rec.doMuon.set_Value_and_Lock(True)
rec.doMuonCombined.set_Value_and_Lock(True)

rec.doTrigger.set_Value_and_Lock(False)

#hardcoded output specification (corresponds to optional flags from runArgs in transform skeleton)
rec.RootNtupleOutput.set_Value_and_Lock("newcommission.ntuple.root")
#rec.RootHistoOutput.set_Value_and_Lock("monitoring.root")
athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
#athenaCommonFlags.SkipEvents = 0

rec.doJiveXML.set_Value_and_Lock(True)
athenaCommonFlags.PoolInputQuery.set_Value_and_Lock("PixelTracks==1&&SCTTracks==1&&TRTTracks==1&&TileMuonFitter>0&&MooreTracks>0&&ConvertedMBoyTracks>0&&NpixSPs>2")

rec.doCBNT.set_Value_and_Lock(False)

if not 'Debug' in dir():
    Debug=False  # change to True to have printout information

if Debug:
    # DEBUG/VERBOSE options
#    rec.doNameAuditor=True
    rec.doPerfMon=True
    rec.doDetailedPerfMon=True
    rec.doDumpProperties=True
#    rec.doDumpTES=True
    rec.OutputLevel = DEBUG

# the main jobOpt
include("RecExCommission/RecExCommission.py")                


from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.monType='BS_ESD'
DQMonFlags.histogramFile.set_Value_and_Lock( "myMonitoringESD.root" )
include ("RecExCommon/RecExCommon_topOptions.py")


svcMgr.EventSelector.CollectionType="ExplicitROOT"
svcMgr.EventSelector.Query=athenaCommonFlags.PoolInputQuery()
