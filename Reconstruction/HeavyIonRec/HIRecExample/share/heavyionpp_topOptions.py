##############################################
# author: Andrzej Olszewski
# 15 September 2008 
##############################################
# to be run on pp data
##############################################
# Detector description
DetDescrVersion = "ATLAS-CSC-01-00-00"
#DetDescrVersion = "ATLAS-GEO-02-01-00"

from RecExConfig.RecFlags import rec

rec.doESD = True
rec.doWriteESD = True
rec.doAOD = True
rec.doWriteAOD = True
rec.doCBNT = True

rec.doHeavyIon = True 

# Heavy Ion specific modifications of reconstruction flags are in
# heavyion_flagsESD.py (heavyion_flagsAOD.py)
# They can be redefined here, e.g.

# But first turn on pp mode
# mainly removes mods on standard reco algorithms
from HIRecExample.HIRecExampleFlags import jobproperties
jobproperties.HIRecExampleFlags.ppMode = True

include ("HIRecExample/heavyion_flagsESD.py")
rec.doRestrictedESD = True

# Other heavy ion specific reconstruction flags are in HIRecExamples/python
# HIRecExampleFlags.py. They can be redefined here, e.g.
# -------------------------------------------
#from HIRecExample.HIRecExampleFlags import jobproperties
#jobproperties.HIRecExampleFlags.doHIJetRec = False
#jobproperties.HIRecExampleFlags.doHIJetAnalysis = False
#from HIGlobal.HIGlobalFlags import jobproperties
#jobproperties.HIGlobalFlags.doHIPixelTracklet = False
               
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =5

#number of Event to process
EvtMax = 5

# INPUT data files
PoolRDOInput = [
    "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/olszewsk/Pythia/SIM/070741/RDO.070741._000001.pool.root.1"
#    "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/olszewsk/Pythia/SIM/067487/RDO.067487._00002.pool.root.1"
    ]

# Output names
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.AthenaCommonFlags.PoolAODOutput = "pythia.aod.pool.root"
jobproperties.AthenaCommonFlags.PoolESDOutput = "pythia.esd.pool.root"
jobproperties.Rec.RootNtupleOutput            = "pythia.cbnt.root"

include( "HIRecExample/HeavyIonFlagsConfig.py" )

include ("RecExCommon/RecExCommon_topOptions.py")

include( "HIRecExample/HeavyIonPostOptionsConfig.py" )
