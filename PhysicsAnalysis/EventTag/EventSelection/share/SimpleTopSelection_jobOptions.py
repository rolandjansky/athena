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
EvtMax = 500000

# Use the TAG that you just produce to select the events
# define also the selection criteria
PoolTAGInput = ["TAG.pool.root"]

#The pre-defined simple Bselections
include ( "EventSelection/SemiLeptonicTtbarTagSelection_jobOptions.py" )
include ( "EventSelection/FullyLeptonicTtbarTagSelection_jobOptions.py" )
include ( "EventSelection/FullyHadronicTtbarTagSelection_jobOptions.py" )
include ( "EventSelection/SingleTopTagSelection_jobOptions.py" )

PoolInputQuery=SemiLeptonicTtbar + "||" + FullyLeptonicTtbar + "||" + FullyHadronicTtbar + "||" + SingleTop

#Create a new RDO, ESD and AOD that contain only selected events
PoolAODOutput = "AOD.TagSel.root"

# main reconstruction job jobOptions
include ("RecExCommon/RecExCommon_topOptions.py")
#######################################################

