#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce trackingg ntuple
doTrkNtuple     = False
# --- produce an atlantis data file
doJiveXML       = False
# --- IO setup
doWriteESD      = False
doWriteAOD      = False
# --- write BS
doWriteBS       = True
# --- do auditors ?
doAuditors      = False
doEdmMonitor    = False
doNameAuditor   = False

doPixelTrkNtuple = False
doVP1 = False
#--------------------------------------------------------------
# Load Reconstruction configuration for tools only
#--------------------------------------------------------------

# --- setup flags with default values
from InDetRecExample.InDetJobProperties import InDetFlagsJobProperty

InDetFlagsJobProperty.preProcessing = False
InDetFlagsJobProperty.newTracking = False
InDetFlagsJobProperty.postProcessing = False
InDetFlagsJobProperty.doTruth = False
InDetFlagsJobProperty.loadTools = False

include( "InDetRecExample/ConfiguredInDetFlags.py" )
InDetTrigFlags = ConfiguredInDetFlags(InDetFlagsJobProperty)


include( "InDetRecExample/ConfiguredInDetKeys.py" )
InDetKeys = ConfiguredInDetKeys()

#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------

DetDescrVersion = "ATLAS-DC3-07"

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
  
include("InDetRecExample/InDetRec_all.py")
if 'doWriteBS' in dir() and doWriteBS:
  # --- load writing BS file
  include ("InDetTrigRecExample/InDetTrigWriteBS_jobOptions.py")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# --- Number of events to be processed (default is 10)
theApp.EvtMax  = 100
# --- RDO file
ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]

