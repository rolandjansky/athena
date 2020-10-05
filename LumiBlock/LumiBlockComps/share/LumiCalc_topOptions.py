########################################
# Job to run RecExCommon, schedule
# LumiBlockMetaDataTool 
# but don't write out any AOD outputfile

# the Input AOD File
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput = [ "/afs/cern.ch/user/r/radbal/scratch0/data/TopMix/user.RichardHawkings.0108175.topmix_Muon.AOD.v4._00057.pool.root" ]

# the EvtMax
if not vars().has_key('EvtMax'): EvtMax = -1
athenaCommonFlags.EvtMax=EvtMax

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.OutputLevel = INFO
#GoodRunsListSelectorTool.GoodRunsListVec = [ 'luminosity.xml' ]
#ToolSvc.GoodRunsTool.BlackRunsListVec = [ 'TTbarBlackRunsList.xml' ]

# add LumiBlockMetaDataTool and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
LumiBlockMetaDataTool.OutputLevel = DEBUG

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


# This is need for PANDA Grid job
#xfile = open('dummy.txt','w')
#xfile.write('a')
#xfile.close()
