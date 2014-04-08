DetDescrVersion="ATLAS-GEO-02-01-00" # Set your geometry here
#ConditionsTag="COMCOND-REPC-003-00" # Set the conditions tag here;not needed for AOD
#------------------------------------------------------------
# These 4 lines are needed for real data
#------------------------------------------------------------
#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_data()
#from IOVDbSvc.CondDB import conddb

doTrigger = False # Need to check
EvtMax=-1 # number of event to process

# include your algorithm job options here
UserAlgs=[ "LumiBlock_LumiCalc_GRL.py" ]

# the Input AOD File
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput = [ "/afs/cern.ch/atlas/maxidisk/d36/topmix/user.RichardHawkings.0108175.topmix_Muon.AOD.v5/user.RichardHawkings.0108175.topmix_Muon.AOD.v4._00014.pool.root",
                                   "/afs/cern.ch/atlas/maxidisk/d36/topmix/user.RichardHawkings.0108175.topmix_Muon.AOD.v5/user.RichardHawkings.0108175.topmix_Muon.AOD.v4._00015.pool.root"]

# Output log setting; this is for the framework in general
# You may over-ride this in your job options for your algorithm
OutputLevel = INFO

# Read settings; for performance DPD set ESD to true
readRDO = False
readESD = False
readAOD = True

# Write settings; keep all of these to false.
# Control the writing of your own n-tuple in the alg's job options
doCBNT = False
doWriteESD = False
doWriteAOD = False
doWriteTAG = False

doDPD = True
doFileMetaData = True

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

# this switches off performance monitoring ntuples creation
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False
