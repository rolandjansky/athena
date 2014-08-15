#######################
# standalone options for running ESD->AOD with TrigEgammaMonitoring locally
#
# - configures most things by itself (tested on rel16/17 data ESD 
# -- this takes quite a while for the first time (patience)
# - reads from esd.pool.root and writes to monitor.root
# - state(110713): still produces a few warnings that might be configured away, but is functional (and fast)
# - update(5th Mar 13): added performance monitoring
#######################

# set basic job properties
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.FilesInput = ['esd.pool.root']
jobproperties.AthenaCommonFlags.EvtMax=-1

# setup performance monitoring
from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

# get handle on rec
from RecExConfig.RecFlags import rec
rec.OutputLevel = INFO

# disable physics output
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)

# set up histogram output
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='monitor.root' OPT='RECREATE'"]

# load monitoring flags
if not 'HLTMonFlags' in dir():
  from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
if not 'DQMonFlags' in dir():
  from AthenaMonitoring.DQMonFlags import DQMonFlags

# toggle slice monitoring on/off
if DQMonFlags.monManEnvironment == 'tier0ESD':
  HLTMonFlags.doCalo     = False
  HLTMonFlags.doGeneral  = False
  HLTMonFlags.doBjet     = False
  HLTMonFlags.doBphys    = False
  HLTMonFlags.doMET      = False
  HLTMonFlags.doJet      = False
  HLTMonFlags.doEgamma   = True
  HLTMonFlags.doMuon     = False
  HLTMonFlags.doIDtrk    = False
  HLTMonFlags.doTau      = False
  HLTMonFlags.doMinBias  = False
  HLTMonFlags.doDump     = False
  HLTMonFlags.doOfflineTauTTP = False
  HLTMonFlags.doIDJpsiMon = False
else :
  print 'HLTMonitoring_topOptions.py: environment is not tier0ESD'
  print 'HLTMonitoring_topOptions.py: switching all tools off...'
  HLTMonFlags.doGeneral = False
  HLTMonFlags.doMonTier0 = False

# add monitoring tools
include("TrigHLTMonitoring/addMonTools.py")
ToolSvc.HLTEgammaNavSigTEMon.OutputLevel = INFO

# configure TrigEgammaMonitoring ()
from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaNavSigTEMonTool as TrigEgammaMon
TrigEgammaMon.doActiveChain = True
TrigEgammaMon.doActiveTe = True
TrigEgammaMon.doAllTe = False


# add RecExCommon - main workhorse
include("RecExCommon/RecExCommon_topOptions.py")
