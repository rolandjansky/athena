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

# Parse command line options and setup input/output files
# Note that you can setup the slimming via the commandline
# by setting TestType = "string", where string can be:
# RS<stream> - remove the specified stream
# RF<feature> - remove the specified feature
# Squeeze - remove intermediate elements
# Or any combination of the above
if not acf.EvtMax.is_locked():
  acf.EvtMax = 10
if not ('TestType' in dir()):
  TestType = 'NoSlim'
if len(TestType) > 0:
  TestType = '_' + TestType


# used RDOtoESDAOD test which is more likely to exist
if not ('athenaCommonFlags.PoolAODInput' in dir()):
  acf.PoolAODInput = ['../AthenaTrigRDOtoESDAOD/AOD.pool.root']
if not ('athenaCommonFlags.PoolAODOutput' in dir()):
  acf.PoolAODOutput='AOD'+TestType+'.pool.root'

acf.FilesInput=acf.PoolAODInput()


if TestType.startswith('_'):
  TestType = TestType.replace('_', '', 1)

# Setup slimming
include ("TrigNavTools/TrigNavigationSlimming_topOptions.py")
slimmerAlg.WriteTree = 1

# check if squeezing is needed
if TestType.count('Squeeze') > 0:
  slimmerAlg.Squeeze = 1
  TestType = TestType.replace('Squeeze', '')

# check feature and stream removal
lastLength = len(TestType) - 1
while lastLength < len(TestType):
  lastLength = len(TestType)
  if TestType.startswith('RF'):
    TestType = TestType.replace('RF', '', 1)
    feature = TestType.split('RF')[0].split('RS')[0]
    TestType = TestType.replace(feature, '', 1)
    slimmerAlg.FeatureExclusionList = slimmerAlg.FeatureExclusionList + [ feature ]
  if TestType.startswith('RS'):
    TestType = TestType.replace('RS', '', 1)
    stream = TestType.split('RF')[0].split('RS')[0]
    TestType = TestType.replace(stream, '', 1)
    slimmerAlg.StreamExclusionList = slimmerAlg.StreamExclusionList + [ stream ]
    slimmerAlg.ProtectOtherStreams = 1
  
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


