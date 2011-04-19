#####################################################
#DetDescrVersion="ATLAS-XXX-...â
doWriteTAG = False
doWriteESD = False
doESD = False
readAOD = True
doAOD = False
doWriteAOD = False
# read the TAG as input to the job
readTAG = True
EvtMax = 2000000
# Use the TAG to select the events - define also the selection criteria
PoolTAGInput = ["TAG.akira.lumi1E31.pool.root"]
PoolInputQuery="NLooseMuon>0 || NLooseElectron>0"
#Create a new RDO, ESD and AOD that contain only selected events
PoolAODOutput = "AOD.TagSel.root"
# DPD Maker
UserAlgs=[ "AODtoDPD_jobOptions.py" ]
# main reconstruction job jobOptions
include ("RecExCommon/RecExCommon_topOptions.py")
#######################################################
