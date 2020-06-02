########################################
# Job to run RecExCommon, schedule
# LumiBlockMetaDataTool 
# but don't write out any AOD outputfile

########################################

include ("AMAAthena/Trigger_jobOptions.py")

########################################

# the Input AOD File
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput = [ "/afs/cern.ch/user/r/radbal/scratch0/data/TopMix/user.RichardHawkings.0108175.topmix_Muon.AOD.v4._00057.pool.root" ]

# the EvtMax
if not vars().has_key('EvtMax'): EvtMax = -1
athenaCommonFlags.EvtMax=EvtMax

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += TriggerRegistryTool()
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.GoodRunsListVec = [ 'grl1.xml', 'grl2.xml', 'grl3.xml' ]
GoodRunsListSelectorTool.BlackRunsListVec = [ 'blacklist1.xml' ]
#GoodRunsListSelectorTool.OutputLevel = DEBUG

# add LumiBlockMetaDataTool and configure
from GoodRunsListsUser.GoodRunsListsUserConf import DummyDumperAlg
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
job += AthSequencer("ModSequence1")
job += AthSequencer("ModSequence2")

## GRL selector
job.ModSequence1 += GRLTriggerSelectorAlg('GRLTriggerAlg1')
job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = ['grl1']
job.ModSequence1.GRLTriggerAlg1.BlackRunsListArray = ['blacklist1']
job.ModSequence1.GRLTriggerAlg1.TriggerSelection = 'EF_mu20'
#job.ModSequence1.GRLTriggerAlg1.OutputLevel = DEBUG

job.ModSequence1 += DummyDumperAlg('DummyDumperAlg1')
# job.ModSequence1.DummyDumperAlg1.RootFileName = 'file1.root'
THistSvc.Output =  {"new DATAFILE='file1.root' TYP='ROOT' OPT='NEW'"};
job.ModSequence1.DummyDumperAlg1.GRLNameVec = [ 'LumiBlocks_GoodDQ0', 'IncompleteLumiBlocks_GoodDQ0' ]

## GRL selector
job.ModSequence2 += GRLTriggerSelectorAlg('GRLTriggerAlg2')
job.ModSequence2.GRLTriggerAlg2.GoodRunsListArray = ['grl2','grl3']
job.ModSequence2.GRLTriggerAlg2.TriggerSelection = 'EF_e20_loose_passEF || EF_e20_loose'
#job.ModSequence2.GRLTriggerAlg2.OutputLevel = DEBUG

job.ModSequence2 += DummyDumperAlg('DummyDumperAlg2')
# job.ModSequence2.DummyDumperAlg2.RootFileName = 'file2.root'
THistSvc.Output =  {"new DATAFILE='file2.root' TYP='ROOT' OPT='NEW'"};
job.ModSequence2.DummyDumperAlg2.GRLNameVec = [ 'LumiBlocks_GoodDQ1', 'IncompleteLumiBlocks_GoodDQ1' ]

# Either use aodtoaod.py as it is
#include ("RecExCommon/aodtoaod.py")

# Or work directly on RecExCommon_topOptions.py
# turn OFF most flags for RecExCommon, no output files
doCBNT=False
doWriteRDO=False
doWriteESD=False
doWriteAOD=False
doAOD=False
doWriteTAG=False 
readAOD=True

# turn ON these 2 flags
doDPD=True
doFileMetaData=True

# execute RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

# reduce POOL verbosity
os.environ['POOL_OUTMSG_LEVEL'] = 'WARNING'
os.environ['CORAL_MSGLEVEL'] = 'WARNING'

# This is need for PANDA Grid job
#xfile = open('dummy.txt','w')
#xfile.write('a')
#xfile.close()

