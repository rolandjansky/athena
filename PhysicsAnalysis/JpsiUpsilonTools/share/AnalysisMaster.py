############################################################
# BPhysAnalysisMaster.py
# These are the master job options for running your analysis
# Include your own job options as shown below ensuring you
# do not overwrite things set here
############################################################

DetDescrVersion="ATLAS-CSC-02-01-00" # Set your geometry here
#ConditionsTag="COMCOND-REPC-003-00" # Set the conditions tag here; not needed for AOD  

#------------------------------------------------------------
# These 4 lines are needed for ESD and real data 
#------------------------------------------------------------ 
#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_data() # only needed for real data
#from IOVDbSvc.CondDB import conddb

doTrigger = False  
EvtMax=10 # number of event to process

# include your algorithm job options here
UserAlgs=[ "RunJpsiExample.py" ] 
PoolAODInput=[ "aod.pool.root" ] 

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

# main jobOption - must always be included
include("RecJobTransforms/UseOracle.py") # DB access
include ("RecExCommon/RecExCommon_topOptions.py")
