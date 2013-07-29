####################################################
#Author - Ketevi A. Assamagan

#DetDescrVersion="ATLAS-XXX-..."

doHist = False
doCBNT = False
doWriteTAG = False
doWriteESD = False
doESD = False
readAOD = True
doAOD = False
doWriteAOD = True
# read the TAG as input to the job
readTAG = True

# Number of Events
EvtMax = 2000

# Use the TAG that you just produce to select the events
# define also the selection criteria
PoolTAGInput = ["TAG.pool.root"]
PoolInputQuery="NLooseMuon>0 || NLooseElectron>0"

#Create a new RDO, ESD and AOD that contain only selected events
PoolAODOutput = "AOD.TagSel.root"

# main reconstruction job jobOptions
include ("RecExCommon/RecExCommon_topOptions.py")
#######################################################



