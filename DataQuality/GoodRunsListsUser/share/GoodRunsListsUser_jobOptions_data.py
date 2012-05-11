DetDescrVersion="ATLAS-GEO-08-00-02" ## Set your geometry here
#ConditionsTag="COMCOND-REPC-003-00" ## Set the conditions tag here;not needed for AOD

#------------------------------------------------------------
# These 4 lines are needed for data, not for mc
#------------------------------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data()
from IOVDbSvc.CondDB import conddb

doTrigger = False ## Need to check
EvtMax=-1 ## number of event to process

## include your algorithm job options here
UserAlgs=[ 
           "GoodRunsListsUser/GoodRunsListsUser_default_data.py"
         ]

## the Input AOD File(s)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput = [
				   "/afs/cern.ch/atlas/maxidisk/d36/data09_900GeV/data09_900GeV.00142383.physics_MinBias.merge.AOD.r988_p62_tid102217_00/AOD.102217._000003.pool.root.1",
				   "/afs/cern.ch/atlas/maxidisk/d36/data09_900GeV/data09_900GeV.00142383.physics_MinBias.merge.AOD.r988_p62_tid102217_00/AOD.102217._000006.pool.root.1",
				 ]

## Output log setting; this is for the framework in general
## You may over-ride this in your job options for your algorithm
OutputLevel = INFO

## Read settings; for performance DPD set ESD to true
readRDO = False
readESD = False
readAOD = True

## Write settings; keep all of these to false.
## Control the writing of your own n-tuple in the alg's job options
doCBNT = False
doWriteESD = False
doWriteAOD = False
doWriteTAG = False

## need these two flags to turn on lumiblockmetadatatool and output metadatastore
doDPD=True
doFileMetaData=True

## main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

