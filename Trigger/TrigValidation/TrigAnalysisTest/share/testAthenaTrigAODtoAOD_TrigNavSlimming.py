# We want to create a new AOD from the existing AOD, only changing
# the navigation structure
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

rec.doCBNT=False
rec.doWriteRDO=False
rec.doWriteESD=False
rec.doWriteAOD=True
rec.doAOD=False
rec.doDPD=False
rec.doWriteTAG=False
rec.readAOD=True


from RecExConfig.RecFlags  import rec
rec.doWriteAOD.set_Value_and_Lock(True)

if not acf.EvtMax.is_locked():
    acf.EvtMax = 10


if not ('PoolAODInput' in dir()):
  acf.PoolAODInput = ['AOD.pool.root']
if not ('PoolAODOutput' in dir()):
  acf.PoolAODOutput='AODSlim.pool.root'

acf.FilesInput=acf.PoolAODInput()

# Setup slimming
include ("TrigNavTools/TrigNavigationSlimming_test.py")

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")




#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()


from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]


include("TriggerTest/TriggerTestCommon.py")



