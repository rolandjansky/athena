#############################################################################
# File: TauCommonDetailsMerger_topOptions.py
# Author: Thomas Burgess <tburgess-at-cern-ch>
# Date: 26-03-2009
# Run like this: athena.py TauCommonDetailsMerger_topOptions.py
#############################################################################

### Ignore geo tag (needed for some old files...)
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Add top algorithms to be run
from TauTools.TauToolsConf import TauCommonDetailsMerger
job += TauCommonDetailsMerger("TauCommonDetailsMerger")
### Set output level threshold (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
job.TauCommonDetailsMerger.OutputLevel = VERBOSE
### Name of TauJet collection, standard is TauRecContainer
job.TauCommonDetailsMerger.TauContainerName="TauRecContainer"
### Set names of tau containers to work on
### (standard is TauRec*)                           
job.TauCommonDetailsMerger.TauDetailsContainerName="TauRecDetailsContainer"
job.TauCommonDetailsMerger.TauExtraDetailsContainerName="TauRecExtraDetailsContainer"
### Set true for  AOD's (since there are no extra details)
### (leaving it off while processing AOD is safe, but StoreGate will print WARNINGS)
job.TauCommonDetailsMerger.SkipExtraDetails = False

### Number of events to be processed, -1 for all events
theApp.EvtMax=-1

### Setup to make new ESD from old (pre rel15) ESD
doCBNT=False
readESD=True
doWriteESD=True
doWriteAOD=False 
doAOD=False
doWriteTag=False
doPerfMon=False
PoolESDInput=['oldEDM_ESD.pool.root']
PoolESDOutput="merged_oldEDM_ESD.pool.root"

# main jobOption

include("RecExCommon/RecExCommon_topOptions.py")

# EOF
